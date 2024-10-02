#include "../../inc/linker/linker.h"
#include "../../inc/linker/parser.h"
#include <iomanip>
#include<iostream>
#include <fstream>
int Linker::link()
{

  int outfilenameind = -1;
  smatch matches;
  for (int i = 1; i < numargs; i++)
  {
    if (i == outfilenameind)
    {
      this->outputFileName = arguments[i];
      continue;
    };
    string arg = arguments[i];
    if (arg == "-o")
    {
      outfilenameind = i + 1;
    }
    else if (arg == "-hex")
      hexa = true;
    else if (arg == "-relocatable")
      reloc = true;
    else if (regex_match(arg, matches, placeRegex))
    {
     unsigned int val=(stoul(matches[2].str().substr(2), nullptr, 16));
      predefinedStartAdr[matches[1]] = val;
    }

    else // U pitanju je ime ulaznog fajla
    {
      inputFiles.push_back(arg);
    }
  }

  if (hexa && reloc)
  {
    cout << "**** LinkError: Bad arguments, can not be set hex and relocatable at the same time! ****" << endl;
    return -1;
  }
  if (!hexa && !reloc)
  {
    cout << "**** LinkError: Bad arguments, either hex or relocatable must be set! ****" << endl;
    return -1;
  }
  int retStatus;
  string line;


  // Obrada ulaznih fajlova
  for (string infilename : inputFiles)
  {
    string filename = "../../src/linker/Helper" + infilename.substr(0, infilename.find('.'))+".txt";
    std::ifstream file;
    file.open(filename);

    if (!file.is_open())
    {
      std::cerr << "Error: Could not open file" << filename << std::endl;
      return -1;
    }

    currentFileName = infilename;

    // Čitajte red po red
    while (std::getline(file, line))
    {

      if (regex_search(line, matches, rg_STLine))
      {

        retStatus = processSTLine(matches[1], matches[2], matches[3], matches[4], matches[5]);
        if (retStatus != 0)
          return retStatus;
      }
      else if (regex_search(line, matches, rg_RTLine))
      {
        retStatus = processRTLine(matches[1], matches[2], matches[3], matches[4], matches[5]);
        if (retStatus != 0)
          return retStatus;
      }
      else if (regex_search(line, matches, rg_SNLine))
      {
        currentSectionName = matches[1];
        Pair newpair;
        newpair.sectionname = currentSectionName;
        newpair.filename = currentFileName;
        order.push_back(newpair);
      }
      else if (regex_search(line, matches, rg_SDLine))
      {
        
        for (int i = 1; i <= 8; i++)
        {
          retStatus = processSDLine(matches[i]);
          if (retStatus != 0)
            return -1;
        }
      }
    }
    
    // Zatvorite fajl
    file.close();
  }

  retStatus = this->findSizeOfSections();
  if (retStatus != 0)
    return retStatus;
  retStatus = this->fillExeFile();
  if (retStatus != 0)
    return retStatus;
  retStatus = this->findValueOfSymbols();
  if (retStatus != 0)
    return retStatus;


  retStatus = this->solveRelocations();
  if (retStatus != 0)
    return retStatus;
  retStatus = this->createRelocatableFile();
  if (retStatus != 0)
    return retStatus;
  this->findSizeOfSumbols();
  if (hexa)
    this->createHexFile();
  if (reloc)
    this->createRelocatableFile();
   


  return 0;
}
int Linker::createHexFile()
{
   string outHelperFileName="../../src/emulator/Helper"+outputFileName;

  ofstream outHelperFile(outHelperFileName);
   if (!outHelperFile.is_open())
  {
    std::cerr << "**** Fail to open .hex file ****" << std::endl;
    return -1;
  }
  ofstream outFile(outputFileName);
  if (!outFile.is_open())
  {
    std::cerr << "**** Fail to open .hex file ****" << std::endl;
    return -1;
  }

  list<string> sectionsOrder;
  for (auto it1 : order)
  {
    auto it2 = std::find(sectionsOrder.begin(), sectionsOrder.end(), it1.sectionname);

    // Proveravamo da li je pronađen
    if (it2 == sectionsOrder.end())
      sectionsOrder.push_back(it1.sectionname);
  }

  int address;
  int nl = 0;
  for (auto iter1 : sectionsOrder)
  {
    address = sectionsStartAdr[iter1];
    string secName = iter1;
    outFile << "#." << secName << endl;
    vector<int> &sectionDatas = data[iter1];

    for (int c : sectionDatas)
    {
      if (nl % 8 == 0)
      {
        if (nl != 0)
        {
          outFile << endl;
          outHelperFile<<endl;
          address += 8;
        }
        outFile << hex << setfill('0') << setw(8) << address << ": ";
        outHelperFile << hex << setfill('0') << setw(8) << address << ":";
      }
      outFile << hex << setw(2) << setfill('0') << c; // static_cast<int>(c)
      outHelperFile << hex << setw(2) << setfill('0') << c;
      if((nl+1)%8!=0) {outFile<<" "; outHelperFile<<":";}
      nl++;
    }
    while(nl % 8 !=0) {
      outHelperFile<<"xx";
      if((nl+1)%8!=0)outHelperFile<<":";
      nl++;
    }

    nl = 0;
    outFile << endl;
    outFile << endl;
    outHelperFile<<endl;
  }

  return 0;
}

int Linker::solveRelocations()
{

  int finallyOffset;
  int finallyValue;
  for (auto r : relocations)
  {
 
    auto key = make_pair(r.fileName, r.sectionName);
    finallyOffset = r.offset + sectionsOffset[key];
    finallyValue = symbolTable[r.symbolName].value + r.addend;

    if(symbolTable[r.symbolName].type==0) finallyValue+=sectionsOffset[key];
    

    vector<int> &mylist = data[r.sectionName];
    for (int i = 0; i < 4; i++)
    {
      mylist[finallyOffset + i] = (((finallyValue) >> i * 8) & 0xff);
    }
  }
  return 0;
}
int Linker::findValueOfSymbols()
{
  int val;
  for (auto it1 : symbolTable)
  {

    SymbolTable &sym = it1.second;
    if (sym.symbolName == "UNDEFINED")
    {
    }
    else if (sym.type == 0)
    {
      val = sectionsStartAdr[sym.symbolName];
      symbolTable[it1.second.symbolName].value = val;
    }
  }

  for (auto it1 : symbolTable)
  {
    if (it1.second.type == 1)
    {
      auto key = make_pair(it1.second.fileName, it1.second.sectionName);
      symbolTable[it1.second.symbolName].value += (symbolTable[it1.second.sectionName].value + sectionsOffset[key]);
    
    }
  }

  return 0;
}
int Linker::fillExeFile()
{ 
  unsigned int maxVal=0; string secWithMaxVal;

  for(auto a: predefinedStartAdr) 
  { 
     unsigned int v=a.second; string sname=a.first;
     sectionsStartAdr[sname]=v;
     if(v>maxVal) {maxVal=v; secWithMaxVal=sname;}
  }
  firstAvailableAdr=maxVal+sectionsSize[secWithMaxVal];
  string secName;
  int offset;
  for (auto c : order)
  {

    secName = c.sectionname;
    auto key = make_pair(c.filename, c.sectionname);
    offset = sectionsOffset[key];
    auto it1 = sectionsStartAdr.find(secName);
    // Odredjivanje pocetne adrese sekcije
    if (it1 == sectionsStartAdr.end())
    {

      // Startna adresa nije odredjena
      auto it2 = predefinedStartAdr.find(secName);

      if (it2 != predefinedStartAdr.end() && hexa)
      {
       
        if (it2->second < firstAvailableAdr)
        {
          cout << "**** LinkerError: section " << secName << " can not be set on adress " << it2->second << "! ****" << endl;
          return -1;
        }
        else{
         
          firstAvailableAdr = it2->second;
         
      }
      }
      sectionsStartAdr[secName] = firstAvailableAdr;
      auto it3 = sectionsSize.find(secName);
      if (it3 != sectionsSize.end()){
        firstAvailableAdr += it3->second;
      }
    }

    vector<int> &dataOfSection = dataOfFilesSections[key];
    for (auto d : dataOfSection)
      data[secName].push_back(d);
  }

  return 0;
}
int Linker::findSizeOfSections()
{

  int size = 0;
  string secName;

  for (list<Pair>::iterator it = order.begin(); it != order.end(); ++it)
  {
    auto key = make_pair(it->filename, it->sectionname);
    secName = it->sectionname;
    size = dataOfFilesSections[key].size();

    auto iter = sectionsSize.find(secName);

    if (iter != sectionsSize.end())
    {
      sectionsOffset[key] = sectionsSize[secName];
      sectionsSize[secName] += size;
    }
    else
    {
      sectionsOffset[key] = 0;
      sectionsSize[secName] = size;
    }
  }

  return 0;
}
int Linker::processSDLine(string arg)
{

  if (arg == "xx")
    return 0;
  int val = stringToDecimalNumber("0x" + arg);

  pair<string, string> key = make_pair(currentFileName, currentSectionName);
  auto it = dataOfFilesSections.find(key);

  if (it != dataOfFilesSections.end())
  {
    // Postoji
    dataOfFilesSections[key].push_back(val & 0xff);
  }
  else
  {
    // Ne postoji
    dataOfFilesSections[key] = std::vector<int>();
    dataOfFilesSections[key].push_back(val & 0xff);
  }

  return 0;
}
int Linker::processRTLine(string _offset, string _type, string _symbolName, string _addend, string _sectionName)
{

  int loffset = stringToDecimalNumber(_offset);
  int ltype = stringToDecimalNumber(_type);
  int laddend = stringToDecimalNumber(_addend);
  struct RelocationTable newreloc;
  newreloc.addend = laddend;
  newreloc.fileName = currentFileName;
  newreloc.offset = loffset;
  newreloc.sectionName = _sectionName;
  newreloc.symbolName = _symbolName;
  newreloc.type = ltype;
  relocations.push_back(newreloc);

  return 0;
}
int Linker::processSTLine(string symname, string symOrSec, string val, string defOrNot, string _sectionName)
{

  int lvalue = stringToDecimalNumber(val);
  int ldefined = stringToDecimalNumber(defOrNot);
  int ltype = stringToDecimalNumber(symOrSec);

  auto it = symbolTable.find(symname);

  if (it != symbolTable.end())
  {
    if (it->second.type == 0)
      return 0;
    if (it->second.defined == 1 && ldefined == 1)
    {
      cout << "**** LinkerError: multiple definiton of symbol " << symname << "! ****" << endl;
      return -1;
    }
    else if (ldefined == 0)
      return 0;
    else
    {
      it->second.fileName = currentFileName;
      it->second.value = lvalue;
      it->second.defined = 1;
      it->second.sectionName = _sectionName;
      return 0;
    }
  }
  else
  {

    struct SymbolTable newsym;
    newsym.symbolName = symname;
    newsym.type = ltype;
    newsym.value = lvalue;
    newsym.defined = ldefined;
    newsym.sectionName = _sectionName;
    newsym.fileName = currentFileName;
    symbolTable[symname] = newsym;

    return 0;
  }
}
int Linker::findSizeOfSumbols()
{

  for (auto &it : symbolTable)
  {
    if (it.second.type == 1)
      it.second.size = 0;
    else
      it.second.size = sectionsSize[it.second.symbolName];
  }

  return 0;
}
int Linker::stringToDecimalNumber(string numberStr)
{
  bool isHex = false;
  if (numberStr.size() > 2 && numberStr.substr(0, 2) == "0x")
  {
    isHex = true;
  }

  if (isHex)
  {
    std::string hexStr = numberStr.substr(2);
    return static_cast<int>(stoll(numberStr, nullptr, 16));
  }
  else
  {
    std::istringstream iss(numberStr);
    int decimalValue;
    iss >> decimalValue;
    return decimalValue;
  }
}
int Linker::createRelocatableFile()
{
  ofstream outFile(outputFileName);
  if (!outFile.is_open())
  {
    std::cerr << "**** Fail to open .o file ****" << std::endl;
    return 1;
  }
 int num=0;
  // Writing SymbolTable into  file
  outFile << "\t\t\t\t\t\t\t\t\tSymbol table: "<<symbolTable.size()<<" Entries" << endl;
  outFile << "Num\t\t\tName\t\t\t\t\tType\t\t\t\tValue\t\t\tSection\t\t\tFileName\t\t\tSize\t\t\tBind\t\t\t Defined" << endl;
  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {

    outFile <<num<<"\t\t\t"<< std::left << it->second.symbolName << "\t\t\t\t" << it->second.type << "\t\t\t\t\t\t" << (0xffffffff & it->second.value) <<"\t\t\t";
    outFile << it->second.sectionName << "\t\t\t\t" << it->second.fileName << "\t\t\t" << hex << setw(4) << setfill('0') <<right<< it->second.size << "\t\t GLO \t\t\t" << it->second.defined << endl;
    num++;
  }
  outFile << endl;
  outFile << endl;
  int numEntriesSH=0; num=0;
  for(auto a: symbolTable)
  {
    if(a.second.type==0 && a.second.symbolName!="UNDEFINED") numEntriesSH++;

  }
  
  // Writing SeactionHeader into file
  outFile << "\t\t\t\t\t\t\t\t\tSeaction header: " << numEntriesSH<< " Entries"<<endl;
  outFile << "Num\t\t\tAddresss\t\t\t\tSize\t\t\tType\t\t\t\t\tName"<< endl;
  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {
     if(it->second.symbolName=="UNDEFINED" || it->second.type==1) continue;
    outFile << num << "\t\t\t\t" << hex<<setw(8)<<setfill('0')<<right<<sectionsStartAdr[it->second.symbolName] << "\t\t\t\t" <<  hex << setw(4) << setfill('0') <<right<<it->second.size <<"\t\t\t";
    outFile << "PROGBITS" << "\t\t\t\t" << it->second.symbolName<<endl;
    num++;
  }

  outFile << endl;
  outFile << endl;
  //Writing Section Data
  int nl = 0;

  list<string> secorder;
  for (auto sn : order)
  {
    auto it = find(secorder.begin(), secorder.end(), sn.sectionname);
    if (it == secorder.end())
      secorder.push_back(sn.sectionname);
  }
  int address = 0;
  for (auto sn : secorder)
  {
    outFile << "#." << sn << endl;
    nl = 0;
    vector<int> &sectionData = data[sn];
    address = sectionsStartAdr[sn];
    for (auto d : sectionData)
    {
      if (nl % 8 == 0)
      {
        if (nl != 0)
          outFile << endl;
        outFile << hex << setw(8) << setfill('0') << address << ": ";
      }
      outFile << hex << setw(2) << right << setfill('0') << d << " ";
      nl++;
      address++;
    }
    outFile << endl;
    outFile << endl;
  }

  outFile << endl;
  outFile << endl;

  return 0;
}
int Linker::readSectionsAddress() {
    for(auto a: sectionsStartAdr) {
      cout<<"Pocetak sekcije "<<a.first<<" je "<<hex<<setw(8)<<setfill('0')<<a.second<<endl;  
       }
       return 0;
   }
  Linker::Linker(int arc, const char *argv[])
  {
 
    numargs = arc;
    arguments = argv;
    hexa = false;
    reloc = false;
    currentFileName="";currentSectionName="";
    firstAvailableAdr=0;
  }