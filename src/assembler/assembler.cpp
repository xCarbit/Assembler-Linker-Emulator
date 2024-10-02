#include "../../inc/assembler/assembler.h"
#include "../../inc/assembler/parser.h"
#include <fstream>
#include <iomanip>
#include <iostream>
using namespace std;

int Assembler::compile()
{
  
  int retStatus = 0;

  ifstream inFile;
  inFile.open("./" + inputFileName);
  if (inFile.is_open() == false)
  {
    cout << "***Can not  open file " << inputFileName << "!***" << endl;
    return -1;
  }
  string line;
  // Dohvataju se linije jedna po jedna
  while (getline(inFile, line))
  {
    smatch matches;
    if (regex_search(line, matches, comment))
    {
     
    }
    else if (regex_search(line, matches, equDirective))
    {

      string sym = matches[1];
      string exp = substringAfterComma(matches[0]);
      retStatus = processEquDirective(sym, exp);
      if (retStatus != 0)
        return retStatus;
    }

    else if (regex_search(line, matches, instrLdRegIndPomSym))
    {
      retStatus = processLdRegIndPomSym(matches[1], matches[2], matches[3]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instStRegIndPomLit))
    {
      retStatus = processStRegIndPomLit(matches[1], matches[2], matches[3]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instStRegInd))
    {
      retStatus = processStRegInd(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instStRegDir))
    {
      retStatus = processStRegDir(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instStMemDir))
    {
      retStatus = processStMemDir(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrLdRegIndPomLit))
    {

      retStatus = processLdRegIndPomLit(matches[1], matches[2], matches[3]);
      if (retStatus != 0)
        return retStatus;
    }

    else if (regex_search(line, matches, emptyLine))
    {
    }
    else if (regex_search(line, matches, instStRegIndPomSym))
    {
     
      retStatus = processStRegIndPomSym(matches[1], matches[2], matches[3]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrLdMemDir))
    {
      retStatus = processLdMemDir(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrLdRegInd))
    {
      retStatus = processLdRegInd(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrLdImm))
    {
      retStatus = porcessLdImm(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrLdRegDir))
    {
      retStatus = processLdRegDir(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrCSRRD))
    {

      retStatus = processCSRRDInstruction(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrCSRWR))
    {

      retStatus = processCSRWRInstruction(matches[1], matches[2]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instAritmLogic))
    {
      retStatus = processAritmLocigInstruction(matches[1], matches[2], matches[3]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instBranches))
    {
      retStatus = processBranchInstruction(matches[1], matches[2], matches[3], matches[4]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instWithOneOperand))
    {
      string instr = matches[1];
      string arg = matches[2];
      retStatus = processInstWithOneOperand(instr, arg);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, instrWithoutOperand))
    {

      string instruction = matches.str(1);
      retStatus = processInstrWithoutOperand(instruction);
      if (retStatus != 0)
        return retStatus;
    }

    else if (regex_search(line, matches, asciiDirective))
    {

      retStatus = processAsciiDirective(matches[1]);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, endDirective))
    {
      string str = matches.str(1);
      retStatus = processEndDirective(str);
      if (retStatus == 0)
        break;
    }
    else if (regex_search(line, matches, symbolDirective))
    {

      string symbol = matches.str(1);
      processSymbolDirective(symbol);
    }
    else if (regex_search(line, matches, globalDirective))
    {

      string symbols = matches.str(1);
      stringstream ss(symbols);

      string symbol;

      while (getline(ss, symbol, ','))
      {
        while (symbol[0] == ' ')
          symbol = symbol.substr(1);
        retStatus = processGlobalDirective(symbol);
        if (retStatus != 0)
          return retStatus;
      }
    }
    else if (regex_search(line, matches, externDirective))
    {

      string symbols = matches.str(1);
      stringstream ss(symbols);

      string symbol;

      while (getline(ss, symbol, ','))
      {
        while (symbol[0] == ' ')
          symbol = symbol.substr(1);
        retStatus = processExternDirective(symbol);
        if (retStatus != 0)
          return retStatus;
      }
    }
    else if (regex_search(line, matches, sectionDirective))
    {

      string secName = matches.str(1);
      retStatus = processSectionDirective(secName);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, skipDirective))
    {

      string lit = matches.str(1);
      retStatus = processSkipDirective(lit);
      if (retStatus != 0)
        return retStatus;
    }
    else if (regex_search(line, matches, wordDirective))
    {
      if (currentSectionName == "")
      {
        cout << "**** AssemblyError-Line " << currentLine << ": word direktive must be in any section! ****" << endl;
        return -1;
      }

      string symbols = matches.str(1);
      stringstream ss(symbols);

      string symbol;

      while (getline(ss, symbol, ','))
      {
        while (symbol[0] == ' ')
          symbol = symbol.substr(1);
        retStatus = processWordDirective(symbol);
        if (retStatus != 0)
          return retStatus;
      }
    }
    else
    {
      cout << "**** Ne poklapa se nijedan regex! ****" << endl;
    }

    this->currentLine++;
  }
  int retstat;
  retStatus = this->processEquPool();
  if (retStatus != 0)
    return retStatus;
  retstat = this->backPatching();
  if (retstat != 0)
    return retstat;
  this->createHelperFile();

  return 0;
}
int Assembler::processEquPool()
{

  int finallyVal = 0;
  string sym = "";
  int val = 0;

  for (auto it1 : equPool)
  {
    finallyVal = 0;
    sym = "";
    for (auto &it4 : symbolTable)
      it4.second.numEquSyms = 0;
    for (auto it2 : it1.second)
    {
      if (isdigit(it2.sym_lit[0]))
      {
        val = stringToDecimalNumber(it2.sym_lit);
        finallyVal += val;
      }
      else
      {
        auto it3 = symbolTable.find(it2.sym_lit);
        if (it3 == symbolTable.end())
        {
          cout << "****AsseblerError: Can not determine value of symbol " << it1.first << "!****" << endl;
          return -1;
        }
        else
        {
          if (it3->second.defined == 0)
          {
            cout << "****AsseblerError: Can not determine value of symbol " << it1.first << "!****" << endl;
            return -1;
          }
          else
          {
            val = it3->second.value;
            for (auto &it5 : symbolTable)
            {
              if (it5.second.idSymbol == it3->second.idSection)
              {
                if (it2.sign == "+")
                  it5.second.numEquSyms++;
                else
                  it5.second.numEquSyms--;
              }
            }
          }
        }

        if (it2.sign == "+")
          finallyVal += val;
        else
          finallyVal -= val;
      }
    }
    for (auto &it6 : symbolTable)
    {
      if (it6.second.typeSymbol == 0 && it6.second.numEquSyms != 0)
      {
        cout << "****AsseblerError: Can not determine value of symbol " << it1.first << "!****" << endl;
        return -1;
      }
    }

    // Treba proveriti da li simbol vec postoji u tabeli simbola
    auto iteration = symbolTable.find(it1.first);
    if (iteration == symbolTable.end())
    {
      SymbolTable newsym;
      newsym.bind = 0;
      newsym.defined = 1;
      newsym.idSection = 0;
      newsym.idSymbol = idSymbol++;
      newsym.nameSymbol = it1.first;
      newsym.size = 0;
      newsym.typeSymbol = 1;
      newsym.value = finallyVal;
      newsym.equ = 1;
      symbolTable[it1.first] = newsym;
    }
    else
    {
      iteration->second.defined = 1;
      iteration->second.idSection = 0;
      iteration->second.value = finallyVal;
      iteration->second.equ = 1;
    }
  }
  return 0;
}
int Assembler::processEquDirective(string sym, string expr)
{
  vector<string> arr = splitString(expr);
  if (arr[0] != "+" && arr[0] != "-")
    arr.insert(arr.begin(), "+");
  auto it = equPool.find(sym);
  if (it == equPool.end())
  {
    equPool[sym] = std::vector<Wrapper>();
  }
  for (int i = 0; i < arr.size(); i += 2)
  {
    Wrapper nw;
    nw.sign = arr[i];
    nw.sym_lit = arr[i + 1];
    nw.processed = false;
    equPool[sym].push_back(nw);
  }
  return 0;
}

int Assembler::processStRegIndPomSym(string arg1, string arg2, string arg3)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction st must be in any section! ****" << endl;
    return -1;
  }

  int gprindData;
  int gprindAdr;
  if (arg1 == "%pc")
    gprindData = 15;
  else if (arg1 == "%sp")
    gprindData = 14;
  else
    gprindData = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    gprindAdr = 15;
  else if (arg2 == "%sp")
    gprindAdr = 14;
  else
    gprindAdr = stoi(arg2.substr(2));

  PoolTable newval;
  newval.startAddresInstr = locationCounter; // NOVO
  newval.name = arg3;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;
  newval.RegIndPomSym = true;
  newval.type = 1;

  valuesForPool.push_back(newval);

  sectionTable[currentSectionName].push_back(0x80);
  sectionTable[currentSectionName].push_back(((gprindAdr << 4) & 0xff) | 0x0);
  sectionTable[currentSectionName].push_back(((gprindData << 4) & 0xff) | 0x0);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}

int Assembler::processStRegIndPomLit(string arg1, string arg2, string arg3)
{
  
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction st must be in any section! ****" << endl;
    return -1;
  }

  int gprindData;
  int gprindAdr;
  if (arg1 == "%pc")
    gprindData = 15;
  else if (arg1 == "%sp")
    gprindData = 14;
  else
    gprindData = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    gprindAdr = 15;
  else if (arg2 == "%sp")
    gprindAdr = 14;
  else
    gprindAdr = stoi(arg2.substr(2));

  int val = stringToDecimalNumber(arg3);
  if (canRepresentOn12Bits(val) == false)
  {
    cout << "**** AssemblyError-Line " << currentLine << ": " << arg3 << " can't be represented into 12 bits! ****" << endl;
    return -1;
  }

  sectionTable[currentSectionName].push_back(0x80);
  sectionTable[currentSectionName].push_back(((gprindAdr & 0xf) << 4) | 0x0);
  sectionTable[currentSectionName].push_back(((gprindData & 0xf) << 4) | (val & 0xf));
  sectionTable[currentSectionName].push_back(val >> 4);

  locationCounter += 4;

  return 0;
}

int Assembler::processStRegInd(string arg1, string arg2)
{
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction st must be in any section! ****" << endl;
    return -1;
  }

  int gprindData;
  int gprindAdr;
  if (arg1 == "%pc")
    gprindData = 15;
  else if (arg1 == "%sp")
    gprindData = 14;
  else
    gprindData = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    gprindAdr = 15;
  else if (arg2 == "%sp")
    gprindAdr = 14;
  else
    gprindAdr = stoi(arg2.substr(2));

  sectionTable[currentSectionName].push_back(0x80);
  sectionTable[currentSectionName].push_back(((gprindAdr & 0xf) << 4) | 0x0);
  sectionTable[currentSectionName].push_back(((gprindData & 0xf) << 4) | 0x0);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;
  return 0;
}
int Assembler::processStRegDir(string arg1, string arg2)
{
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction st must be in any section! ****" << endl;
    return -1;
  }

  int gprindDst;
  int gprindSrc;
  if (arg1 == "%pc")
    gprindSrc = 15;
  else if (arg1 == "%sp")
    gprindSrc = 14;
  else
    gprindSrc = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    gprindDst = 15;
  else if (arg2 == "%sp")
    gprindDst = 14;
  else
    gprindDst = stoi(arg2.substr(2));

  sectionTable[currentSectionName].push_back(0x91);
  sectionTable[currentSectionName].push_back(((gprindDst & 0xf) << 4) | (gprindSrc & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;
  return 0;
}
int Assembler::processStMemDir(string arg1, string arg2)
{
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction st must be in any section! ****" << endl;
    return -1;
  }
  int gprind;
  if (arg1 == "%pc")
    gprind = 15;
  else if (arg1 == "%sp")
    gprind = 14;
  else
    gprind = stoi(arg1.substr(2));

  sectionTable[currentSectionName].push_back(0x82);
  sectionTable[currentSectionName].push_back(0xf0);
  sectionTable[currentSectionName].push_back(((gprind & 0xf) << 4) | 0x0);
  sectionTable[currentSectionName].push_back(0x00);

  PoolTable newval;
  newval.startAddresInstr = locationCounter;
  newval.name = arg2;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;

  if (isdigit(arg2[0]))
  {
    newval.type = 0;
  }
  else
  {
    newval.type = 1;
  }

  valuesForPool.push_back(newval);

  locationCounter += 4;

  return 0;
}
int Assembler::processLdRegIndPomSym(string arg1, string arg2, string arg3)
{
  
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }
  int gprindAdr;
  if (arg1 == "%pc")
    gprindAdr = 15;
  else if (arg1 == "%sp")
    gprindAdr = 14;
  else
    gprindAdr = stoi(arg1.substr(2));

  int gprindDst;
  if (arg3 == "%pc")
    gprindDst = 15;
  else if (arg3 == "%sp")
    gprindDst = 14;
  else
    gprindDst = stoi(arg3.substr(2));

  PoolTable newval;
  newval.startAddresInstr = locationCounter; // NOVO
  newval.name = arg2;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;
  newval.RegIndPomSym = true;
  newval.type = 1;

  valuesForPool.push_back(newval);

  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((gprindDst << 4) & 0xff) | gprindAdr);
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}

int Assembler::processLdRegIndPomLit(string arg1, string arg2, string arg3)
{
  
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }
  int val = stringToDecimalNumber(arg2);
  if (canRepresentOn12Bits(val) == false)
  {
    cout << "**** AssemblyError-Line " << currentLine << ": " << arg2 << " can't be represented into 12 bits! ****" << endl;
    return -1;
  }
  int gprindAdr;
  if (arg1 == "%pc")
    gprindAdr = 15;
  else if (arg1 == "%sp")
    gprindAdr = 14;
  else
    gprindAdr = stoi(arg1.substr(2));

  int gprindDst;
  if (arg3 == "%pc")
    gprindDst = 15;
  else if (arg3 == "%sp")
    gprindDst = 14;
  else
    gprindDst = stoi(arg3.substr(2));

  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((gprindDst & 0xf) << 4) | (gprindAdr & 0xf));
  sectionTable[currentSectionName].push_back((val & 0x0000000f));
  sectionTable[currentSectionName].push_back((val >> 4) & 0x000000ff);

  locationCounter += 4;

  return 0;
}
int Assembler::processLdRegInd(string arg1, string arg2)
{
  

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }

  int regindAdr;
  int regindDst;
  if (arg1 == "%pc")
    regindAdr = 15;
  else if (arg1 == "%sp")
    regindAdr = 14;
  else
    regindAdr = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    regindDst = 15;
  else if (arg2 == "%sp")
    regindDst = 14;
  else
    regindDst = stoi(arg2.substr(2));

  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((regindDst & 0xf) << 4) | (regindAdr & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}
int Assembler::processLdMemDir(string arg1, string arg2)
{
  

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }
  int gprind;

  if (arg2 == "%pc")
    gprind = 15;
  else if (arg2 == "%sp")
    gprind = 14;
  else
    gprind = stoi(arg2.substr(2));

  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((gprind & 0xf) << 4) | 0xf);
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  PoolTable newval;
  newval.startAddresInstr = locationCounter; // NOVO
  newval.name = arg1;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;

  if (isdigit(arg1[0]))
  {
    newval.type = 0;
  }
  else
  {
    newval.type = 1;
  }
  valuesForPool.push_back(newval);

  locationCounter += 4;

  
  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((gprind & 0xf) << 4) | (gprind & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}
int Assembler::processLdRegDir(string arg1, string arg2)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }

  int regindSrc;
  int regindDst;
  if (arg1 == "%pc")
    regindSrc = 15;
  else if (arg1 == "%sp")
    regindSrc = 14;
  else
    regindSrc = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    regindDst = 15;
  else if (arg2 == "%sp")
    regindDst = 14;
  else
    regindDst = stoi(arg2.substr(2));

  sectionTable[currentSectionName].push_back(0x91);
  sectionTable[currentSectionName].push_back(((regindDst & 0xf) << 4) | (regindSrc & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}
int Assembler::porcessLdImm(string arg1, string arg2)
{
 
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction ld must be in any section! ****" << endl;
    return -1;
  }

  int regind;
  if (arg2 == "%pc")
    regind = 15;
  else if (arg2 == "%sp")
    regind = 14;
  else
    regind = stoi(arg2.substr(2));

  PoolTable newval;
  newval.startAddresInstr = locationCounter; 
  newval.name = arg1;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;

  if (isdigit(arg1[0]))
  {
    newval.type = 0;
  }
  else
  {
    newval.type = 1;
  }

  valuesForPool.push_back(newval);

  sectionTable[currentSectionName].push_back(0x92);
  sectionTable[currentSectionName].push_back(((regind & 0xf) << 4) | 0xf); // Ispravka
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}
int Assembler::processCSRWRInstruction(string arg1, string arg2)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction csrrd must be in any section! ****" << endl;
    return -1;
  }

  int gprind;
  int csrind;
  if (arg1 == "%pc")
    gprind = 15;
  else if (arg1 == "%sp")
    gprind = 14;
  else
    gprind = stoi(arg1.substr(2));

  if (arg2 == "status")
    csrind = 0;
  if (arg2 == "handler")
    csrind = 1;
  if (arg2 == "cause")
    csrind = 2;

  sectionTable[currentSectionName].push_back(0x94);
  sectionTable[currentSectionName].push_back(((csrind & 0xf) << 4) | (gprind & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);

  locationCounter += 4;

  return 0;
}
int Assembler::processCSRRDInstruction(string arg1, string arg2)
{
  
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction csrrd must be in any section! ****" << endl;
    return -1;
  }

  int csrind = -1;
  int gprind = -1;
  if (arg1 == "status")
    csrind = 0;
  if (arg1 == "handler")
    csrind = 1;
  if (arg1 == "cause")
    csrind = 2;

  if (arg2 == "%pc")
    gprind = 15;
  else if (arg2 == "%sp")
    gprind = 14;
  else
    gprind = stoi(arg2.substr(2));

  

  sectionTable[currentSectionName].push_back(0x90);
  sectionTable[currentSectionName].push_back(((gprind & 0xf) << 4) | (csrind & 0xf));
  sectionTable[currentSectionName].push_back(0x00);
  sectionTable[currentSectionName].push_back(0x00);
  locationCounter += 4;

  return 0;
}
int Assembler::processAritmLocigInstruction(string inst, string arg1, string arg2)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction " << inst << " must be in any section! ****" << endl;
    return -1;
  }

  int regind1;
  int regind2;
  if (arg1 == "%pc")
    regind1 = 15;
  else if (arg1 == "%sp")
    regind1 = 14;
  else
    regind1 = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    regind2 = 15;
  else if (arg2 == "%sp")
    regind2 = 14;
  else
    regind2 = stoi(arg2.substr(2));

  if (inst == "xchg")
  {
    sectionTable[currentSectionName].push_back(0x40);
    sectionTable[currentSectionName].push_back((regind1 & 0xf));
    sectionTable[currentSectionName].push_back((regind2 & 0xf) << 4);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  else if (inst == "add" || inst == "sub" || inst == "mul" || inst == "div")
  {

    if (inst == "add")
      sectionTable[currentSectionName].push_back(0x50);
    else if (inst == "sub")
      sectionTable[currentSectionName].push_back(0x51);
    else if (inst == "mul")
      sectionTable[currentSectionName].push_back(0x52);
    else if (inst == "div")
      sectionTable[currentSectionName].push_back(0x53);

    sectionTable[currentSectionName].push_back(((regind2 & 0xf) << 4) | (regind2 & 0xf));
    sectionTable[currentSectionName].push_back((regind1 & 0xf) << 4);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  else if (inst == "and" || inst == "or" || inst == "xor")
  {

    if (inst == "and")
      sectionTable[currentSectionName].push_back(0x61);
    else if (inst == "or")
      sectionTable[currentSectionName].push_back(0x62);
    else if (inst == "xor")
      sectionTable[currentSectionName].push_back(0x63);

    sectionTable[currentSectionName].push_back(((regind2 & 0xf) << 4) | (regind1 & 0xf));
    sectionTable[currentSectionName].push_back((regind2 & 0xf) << 4);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  else if (inst == "shl" || inst == "shr")
  {
    if (inst == "shl")
      sectionTable[currentSectionName].push_back(0x70);
    else if (inst == "shr")
      sectionTable[currentSectionName].push_back(0x71);

    sectionTable[currentSectionName].push_back(((regind2 & 0xf) << 4) | (regind2 & 0xf));
    sectionTable[currentSectionName].push_back((regind1 & 0xf) << 4);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  return 0;
}
int Assembler::processBranchInstruction(string inst, string arg1, string arg2, string arg3)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction " << inst << " must be in any section! ****" << endl;
    return -1;
  }

  int regind1;
  int regind2;
  if (arg1 == "%pc")
    regind1 = 15;
  else if (arg1 == "%sp")
    regind1 = 14;
  else
    regind1 = stoi(arg1.substr(2));

  if (arg2 == "%pc")
    regind2 = 15;
  else if (arg2 == "%sp")
    regind2 = 14;
  else
    regind2 = stoi(arg2.substr(2));

  if (inst == "beq")
    sectionTable[currentSectionName].push_back(0x39);
  else if (inst == "bne")
    sectionTable[currentSectionName].push_back(0x3A);
  else if (inst == "bgt")
    sectionTable[currentSectionName].push_back(0x3B);

  sectionTable[currentSectionName].push_back((0xf << 4 | (regind1 & 0xf)));
  sectionTable[currentSectionName].push_back((regind2 & 0xff) << 4 | 0x0);
  sectionTable[currentSectionName].push_back(0x00);
  PoolTable newval;
  newval.startAddresInstr = locationCounter; 
  newval.name = arg3;
  newval.sectionName = currentSectionName;
  newval.offset = locationCounter + 2;

  if (isdigit(arg3[0]))
  {
    newval.type = 0;
  }
  else
  {
    newval.type = 1;
  }

  valuesForPool.push_back(newval);

  locationCounter += 4;

  return 0;
}
int Assembler::processInstWithOneOperand(string instr, string arg)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction " << instr << " must be in any section! ****" << endl;
    return -1;
  }
  int regind;

  if (instr == "push")
  {

    // sub sp,$4 st gpr, mem[r14]

    if (arg == "%pc")
    {
      regind = 15;
    }
    if (arg == "%sp")
    {
      regind = 14;
    }
    if (arg.substr(0, 2) == "%r")
      regind = stoi(arg.substr(2));

    sectionTable[currentSectionName].push_back(0x91);
    sectionTable[currentSectionName].push_back(0xee);
    sectionTable[currentSectionName].push_back(0x0c);
    sectionTable[currentSectionName].push_back(0xff);
    sectionTable[currentSectionName].push_back(0x80);
    sectionTable[currentSectionName].push_back(0xe0);
    sectionTable[currentSectionName].push_back(0xff & (regind << 4));
    sectionTable[currentSectionName].push_back(0x00);
    locationCounter += 8;
  }
  else if (instr == "pop")
  {

    if (arg == "%pc")
    {
      regind = 15;
    }
    if (arg == "%sp")
    {
      regind = 14;
    }
    if (arg.substr(0, 2) == "%r")
      regind = stoi(arg.substr(2));

    sectionTable[currentSectionName].push_back(0x92);
    sectionTable[currentSectionName].push_back(((0xff & regind) << 4) | 0xE);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);

    sectionTable[currentSectionName].push_back(0x91);
    sectionTable[currentSectionName].push_back(0xee);
    sectionTable[currentSectionName].push_back(0x04);
    sectionTable[currentSectionName].push_back(0x00);
    locationCounter += 8;
  }
  else if (instr == "not")
  {

    if (arg == "%pc")
    {
      regind = 15;
    }
    if (arg == "%sp")
    {
      regind = 14;
    }
    if (arg.substr(0, 2) == "%r")
      regind = stoi(arg.substr(2));

    sectionTable[currentSectionName].push_back(0x60);
    sectionTable[currentSectionName].push_back(((regind << 4) & 0xff) | regind);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  else if (instr == "jmp")
  {

    PoolTable newval;
    newval.startAddresInstr = locationCounter; // NOVO
    newval.name = arg;
    newval.sectionName = currentSectionName;
    newval.offset = locationCounter + 2;

    if (isdigit(arg[0]))
    {
      newval.type = 0;
    }
    else
    {
      newval.type = 1;
    }

    valuesForPool.push_back(newval);

    sectionTable[currentSectionName].push_back(0x38);
    sectionTable[currentSectionName].push_back(0xF0);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  else if (instr == "call")
  {
    sectionTable[currentSectionName].push_back(0x21);
    sectionTable[currentSectionName].push_back(0xf0);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);


    PoolTable newval;
    newval.startAddresInstr = locationCounter; // Novo
    newval.name = arg;
    newval.sectionName = currentSectionName;
    newval.offset = locationCounter + 2;

    if (isdigit(arg[0]))
    {
      newval.type = 0;
    }
    else
    {
      newval.type = 1;
    }

    valuesForPool.push_back(newval);
    locationCounter += 4;
  }
  return 0;
}
int Assembler::processInstrWithoutOperand(string instr)
{

  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": instruction " << instr << " must be in any section! ****" << endl;
    return -1;
  }

  if (instr == "halt")
  {
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  // Instruction software interrupt
  else if (instr == "int")
  {
    // int
    sectionTable[currentSectionName].push_back(0x10);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    locationCounter += 4;

  }
  else if (instr == "iret")
  {
    // add sp, #8
    sectionTable[currentSectionName].push_back(0x91);
    sectionTable[currentSectionName].push_back(0xee);
    sectionTable[currentSectionName].push_back(0x08);
    sectionTable[currentSectionName].push_back(0x00);

    // pop status
    sectionTable[currentSectionName].push_back(0x96);
    sectionTable[currentSectionName].push_back(0x0e);
    sectionTable[currentSectionName].push_back(0x0c);
    sectionTable[currentSectionName].push_back(0xff);

    // pop pc
    sectionTable[currentSectionName].push_back(0x92);
    sectionTable[currentSectionName].push_back(0xfe); 
    sectionTable[currentSectionName].push_back(0x08);
    sectionTable[currentSectionName].push_back(0xff);

    locationCounter += 12;
  }
  else if (instr == "ret")
  {
    sectionTable[currentSectionName].push_back(0x93);
    sectionTable[currentSectionName].push_back(0xfe);
    sectionTable[currentSectionName].push_back(0x04);
    sectionTable[currentSectionName].push_back(0x00);

    locationCounter += 4;
  }
  return 0;
}

int Assembler::processAsciiDirective(string str)
{
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": ascii directive must be in any section! ****" << endl;
    return -1;
  }

  for (char c : str)
  {
    sectionTable[currentSectionName].push_back(static_cast<int>(c)); // Pretvaranje karaktera u ASCII vrednost
    locationCounter += 1;
  }
  return 0;
}
int Assembler::processEndDirective(string str)
{
  symbolTable[currentSectionName].size = locationCounter;
  locationCounter = 0;
  return 0;
}

int Assembler::processSymbolDirective(string symbol)
{


  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": symbol must be defined in any section! ****" << endl;
    return -1;
  }
  auto it = symbolTable.find(symbol);
  if (it != symbolTable.end())
  {

    // Symbol already declared
    it->second.value = locationCounter;
    it->second.defined = 1;
    it->second.idSection = getIdCurrentSection();
  }
  else
  {

    SymbolTable newsym;
    newsym.bind = 0;
    newsym.idSection = getIdCurrentSection();
    newsym.value = locationCounter; // oznacava da simbol nije definisan
    newsym.idSymbol = idSymbol++;
    newsym.nameSymbol = symbol;
    newsym.typeSymbol = 1;
    newsym.size = 0;
    newsym.defined = 1;
    newsym.equ = 0;
    symbolTable[symbol] = newsym;
  }
  return 0;
}

int Assembler::processSkipDirective(string lit)
{
  
  int numOfBytes = stringToDecimalNumber(lit);
  if (currentSectionName == "")
  {
    cout << "**** AssemblyError-Line " << currentLine << ": skip direktive must be in any section! ****" << endl;
    return -1;
  }

  auto it = sectionTable.find(currentSectionName);
  if (it == sectionTable.end())
  {
    sectionTable.insert(make_pair(currentSectionName, vector<int>()));
  }

  for (int i = 0; i < numOfBytes; i++)
  {
    sectionTable[currentSectionName].push_back(0 & 0xff);
  }

  locationCounter += numOfBytes;
  return 0;
}
int Assembler::processWordDirective(string symOrLit)
{

  smatch matches;

  if (isalpha(symOrLit[0]))
  {
    auto it = symbolTable.find(symOrLit);
    RelocationTable newReloc;
    newReloc.sectionName = currentSectionName;
    newReloc.offset = locationCounter;
    newReloc.type = 32; // corection on 4 bytes
    if (it->second.bind == 1)
    {
      newReloc.symbolName = symOrLit;
      newReloc.addend = 0;
    }
    else
    {
      string secName = "";
      auto iter1 = symbolTable.find(symOrLit);

      for (auto iter2 = symbolTable.begin(); iter2 != symbolTable.end(); iter2++)
      {
        if (iter2->second.typeSymbol == 0 && iter2->second.idSection == iter1->second.idSection)
        {
          secName = iter2->second.nameSymbol;
          break;
        }
      }
      newReloc.addend = iter1->second.value;
      newReloc.symbolName = secName;
    }
    relocationTable.push_back(newReloc);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
    sectionTable[currentSectionName].push_back(0x00);
  }
  else
  {

    int valueOfLit = stringToDecimalNumber(symOrLit);
    sectionTable[currentSectionName].push_back(valueOfLit & 0xff);
    sectionTable[currentSectionName].push_back((valueOfLit >> 8) & 0xff);
    sectionTable[currentSectionName].push_back((valueOfLit >> 16) & 0xff);
    sectionTable[currentSectionName].push_back((valueOfLit >> 24) & 0xff);
  }
  locationCounter += 4;

  return 0;
}

int Assembler::processGlobalDirective(string sym)
{
  auto elem = symbolTable.find(sym);

  if (elem != symbolTable.end())
  {
    // Simbol vec postoji u tabeli
    elem->second.bind = 1;
  }
  else
  {
    // Simbol ne postoji u  tabeli simbola - samo ubacujemo simbol u tabelu simbola
    // Ne znamo vrednost simbola, sekciju kojoj pripada jer global nije definicija
    SymbolTable newsym;
    newsym.bind = 1;
    newsym.idSection = 0;
    newsym.value = 0; 
    newsym.idSymbol = idSymbol++;
    newsym.nameSymbol = sym;
    newsym.typeSymbol = 1;
    newsym.size = 0;
    newsym.defined = 0;
    newsym.equ = 0;
    symbolTable.insert({sym, newsym});
  }
  return 0;
}
int Assembler::processExternDirective(string sym)
{

  return processGlobalDirective(sym);
}

int Assembler::processSectionDirective(string scnName)
{

  auto elem = symbolTable.find(scnName);
  if (elem != symbolTable.end())
  {
    // Sekcija je vec definisana
    cout << "**** AssemblyError-Line " << currentLine << ": Section " << scnName << " has already defined! ****" << endl;
    return -1;
  }
  else
  {

    auto it = symbolTable.find(currentSectionName);
    it->second.size = locationCounter;
    locationCounter = 0;
    currentSectionName = scnName;

    SymbolTable newsec;
    newsec.bind = 0;
    newsec.idSection = idSection++;
    newsec.value = 0;
    newsec.idSymbol = idSymbol++;
    newsec.nameSymbol = scnName;
    newsec.typeSymbol = 0;
    newsec.size = 0;
    newsec.defined = 1;
    newsec.equ = 0;
    symbolTable.insert({scnName, newsec});
    orderSections.push_back(scnName);

    return 0;
  }
}

int Assembler::createHexFile()
{

  ofstream outFile(outputFileName);
  if (!outFile.is_open())
  {
    std::cerr << "**** Fail to open .o file ****" << std::endl;
    return 1;
  }
  // Writing SymbolTable into .txt file
  outFile << "\t\t\t\t\t\t\t\t\tSymbol table: " << symbolTable.size() << " Entries" << endl;
  outFile << "ID\t\t\tName\t\t\t\t\tValue\t\t\t\tType\t\t\tSection\t\tBind\t\tSize\t\t Defined" << endl;
  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {

    outFile << std::left << it->second.idSymbol << "\t\t\t" << std::left << setfill(' ') << std::setw(10) << it->second.nameSymbol << "\t\t\t" << hex << std::right << std::setfill('0') << std::setw(8) << (0xffffffff & it->second.value) << "\t\t\t";

    if (it->second.typeSymbol == 1)
      outFile << std::right << "SYM\t\t\t";
    else
    {
      outFile << std::right << "SCN\t\t\t";
    }
    outFile << std::right << it->second.idSection << "\t\t\t\t\t";
    if (it->second.bind == 1)
    {
      outFile << std::right << "glo\t\t\t";
    }
    else
    {
      outFile << std::right << "loc\t\t\t";
    }

    outFile << hex << std::right << std::setfill('0') << std::setw(4) << (0xffff & it->second.size) << "\t\t\t\t";
    outFile << it->second.defined;
    outFile << endl;
  }
  outFile << endl;
  outFile << endl;

  int num = 0;
  for (auto a : symbolTable)
    if (a.second.nameSymbol != "UNDEFINED" && a.second.typeSymbol == 0)
      num++;
  // Writing Seaction Header file
  outFile << "\t\t\t\t\t\t\t\tSeaction header: " << num << " Entries" << endl;
  num = 0;
  outFile << "Num\t\t\tType\t\t\t\t\tSize\t\t\t\tName" << endl;
  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {
    if (it->second.nameSymbol == "UNDEFINED" || it->second.typeSymbol == 1)
      continue;
    outFile << num << "\t\t\t\t" << "PROGBITS\t\t\t" << hex << setfill('0') << setw(4) << right << it->second.size << "\t\t\t\t" << it->second.nameSymbol << endl;

    num++;
  }
  outFile << endl;
  outFile << endl;

  // Writing RelocationTable into .txt file
  outFile << "\t\t\t\t\t\t\t\t\tRelocation table:" << endl;
  outFile << "Offset\t\t\t\tType\t\t\t\t\t\t\tSymbol\t\t\t\t\t\t\t\tAddend" << "\t\t\t\t\tSection" << endl;
  for (size_t i = 0; i < relocationTable.size(); ++i)
  {
    outFile << hex << setfill('0') << setw(8) << relocationTable[i].offset;
    outFile << "\t\t\t";
    outFile << "Rel_ABS_" + to_string(relocationTable[i].type) << "\t\t\t";
    outFile << relocationTable[i].symbolName << "\t\t\t\t\t\t\t\t\t" << relocationTable[i].addend << "\t\t\t\t\t\t" << relocationTable[i].sectionName << endl;
  }
  outFile << endl;
  outFile << endl;

  // Writing SectionTable into .txt file
  int nl = 0;
  for (auto it = sectionTable.begin(); it != sectionTable.end(); it++)
  {
    string secName = it->first;
    outFile << "#." << secName << endl;
    vector<int> &data = it->second;

    for (int c : data)
    {
      if (nl % 8 == 0)
      {
        if (nl != 0)
          outFile << endl;
        outFile << hex << setfill('0') << setw(8) << nl << ": ";
      }
      outFile << hex << setw(2) << setfill('0') << c << " "; // static_cast<int>(c)
      nl++;
    }
    nl = 0;
    outFile << endl;
    outFile << endl;
    outFile << endl;
  }
  outFile.close();
  return 0;
}
string Assembler::getNameOfSectionForId(int idsec)
{
  string secName = "";
  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {
    if (it->second.typeSymbol == 0 && it->second.idSection == idsec)
    {
      secName = it->second.nameSymbol;
      break;
    }
  }
  return secName;
}
int Assembler::createHelperFile()
{
  string filename = "../../src/linker/Helper" + outputFileName.substr(0, outputFileName.find('.')) + ".txt";
  ofstream outFile(filename);
  if (!outFile.is_open())
  {
    std::cerr << "**** Fail to open .txt file ****" << std::endl;
    return 1;
  }
  // Writing SymbolTable into helper file

  for (map<string, SymbolTable>::iterator it = symbolTable.begin(); it != symbolTable.end(); it++)
  {
    if (it->second.bind == 0 && it->second.typeSymbol == 1)
      continue;
    outFile << "TS-" << it->second.nameSymbol << ":" << it->second.typeSymbol << ":" << it->second.value << ":" << it->second.defined << ":" << getNameOfSectionForId(it->second.idSection) << endl;
  }

  // Writing RelocationTable into helper file
  for (size_t i = 0; i < relocationTable.size(); ++i)
  {
    outFile << "TR-" << relocationTable[i].offset << ":" << relocationTable[i].type << ":" << relocationTable[i].symbolName << ":" << relocationTable[i].addend << ":" << relocationTable[i].sectionName << endl;
  }

  // Writing SectionData into helper file
  for (string lsn : orderSections)
  {
    int nl = 0;
    string secName = lsn;
    outFile << "SN-" << secName << endl;
    vector<int> &data = sectionTable[secName];

    for (int c : data)
    {
      if (nl % 8 == 0)
      {
        if (nl != 0)
          outFile << endl;
        outFile << "SD-";
      }
      outFile << hex << setw(2) << setfill('0') << static_cast<int>(c);
      if ((nl + 1) % 8 != 0)
        outFile << ":";

      nl++;
    }
    while (nl % 8 != 0)
    {
      outFile << "xx";
      if ((nl + 1) % 8 != 0)
        outFile << ":";
      nl++;
    }
    nl = 0;
    outFile << endl;
  }
  return 0;
}
int Assembler::stringToDecimalNumber(string numberStr)
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
int Assembler::getIdCurrentSection()
{

  for (auto it = symbolTable.begin(); it != symbolTable.end(); ++it)
  {
    if (it->second.nameSymbol == currentSectionName)
      return it->second.idSection;
  }

  return -1;
}
int Assembler::backPatching()
{
  for (size_t i = 0; i < valuesForPool.size(); i++)
  {

    string secPolName = valuesForPool[i].sectionName;
    int lc = symbolTable[secPolName].size;
    if (valuesForPool[i].RegIndPomSym == true)
    {
      int of = valuesForPool[i].offset;
      string sname = valuesForPool[i].name; // simbol ciju vrednost treba upisati u instrukciju
      auto ite1 = symbolTable.find(sname);
      if (ite1 == symbolTable.end())
      {
        cout << "****AssemblerError: Can not determine address of registar indirect  with symbol move addressing1!****" << endl;
        return -1;
      }
      else
      {
        if (ite1->second.equ == 0)
        {
          cout << "****AssemblerError: Can not determine address of registar indirect  with symbol move addressing2!****" << endl;
          return -1;
        }
        int va = ite1->second.value;
        if (canRepresentOn12Bits(va) == false)
        {
          cout << "****AssemblerError: Can not determine address of registar indirect  with symbol move addressing3!****" << endl;
          return -1;
        }
        sectionTable[secPolName][of] |= (va & 0xf);
        sectionTable[secPolName][of + 1] |= ((va >> 4) & 0xff);
      }
    }
    else
    {
      if (valuesForPool[i].type == 0) // U bazen treba upisati literal
      {

        int val = stringToDecimalNumber(valuesForPool[i].name);
        sectionTable[secPolName].push_back(val & 0xff);
        sectionTable[secPolName].push_back((val >> 8) & 0xff);
        sectionTable[secPolName].push_back((val >> 16) & 0xff);
        sectionTable[secPolName].push_back((val >> 24) & 0xff);

        int dip = lc - (valuesForPool[i].startAddresInstr + 4); // diplacement
        int off = valuesForPool[i].offset;
        sectionTable[secPolName][off] = sectionTable[secPolName][off] | (dip & 0xf);
        sectionTable[secPolName][off + 1] = ((dip >> 4) & 0xff);

        symbolTable[secPolName].size += 4;
        
      }
      else
      { // U bazen treba upisati vrednost simbola

        string sym = valuesForPool[i].name; // Ime simbola ciju vrednost treba upisati u bazen
        if (symbolTable.find(sym) != symbolTable.end())
        {
          if (symbolTable[sym].equ == 1)
          {
            int val = symbolTable[sym].value;
            sectionTable[secPolName].push_back(val & 0xff);
            sectionTable[secPolName].push_back((val >> 8) & 0xff);
            sectionTable[secPolName].push_back((val >> 16) & 0xff);
            sectionTable[secPolName].push_back((val >> 24) & 0xff);

            int dip = lc - (valuesForPool[i].startAddresInstr + 4); // diplacement
            int off = valuesForPool[i].offset;
            sectionTable[secPolName][off] = sectionTable[secPolName][off] | (dip & 0xf);
            sectionTable[secPolName][off + 1] = ((dip >> 4) & 0xff);

            symbolTable[secPolName].size += 4;
            

            continue;
          }
          int dip = lc - (valuesForPool[i].startAddresInstr + 4); // diplacement
          int off = valuesForPool[i].offset;
          sectionTable[secPolName][off] = sectionTable[secPolName][off] | (dip & 0xf);
          sectionTable[secPolName][off + 1] = ((dip >> 4) & 0xff);

          sectionTable[secPolName].push_back(0x00);
          sectionTable[secPolName].push_back(0x00);
          sectionTable[secPolName].push_back(0x00);
          sectionTable[secPolName].push_back(0x00);

          RelocationTable newReloc;
          newReloc.sectionName = secPolName;
          newReloc.offset = lc;
          newReloc.type = 32; // korekcija na 4 B
          if (symbolTable[sym].bind == 1)
          {
            newReloc.symbolName = sym;
            newReloc.addend = 0;
          }
          else
          {
            string secName = "";
            for (auto iter2 = symbolTable.begin(); iter2 != symbolTable.end(); iter2++)
            {
              if (iter2->second.typeSymbol == 0 && iter2->second.idSection == symbolTable[sym].idSection)
              {
                secName = iter2->second.nameSymbol;
                break;
              }
            }
            newReloc.addend = symbolTable[sym].value;
            newReloc.symbolName = secName;
          }
          relocationTable.push_back(newReloc);
          symbolTable[secPolName].size += 4;

        } // Koristi se simbol koji nije ni definisan ni deklarisan
        else
        {
          cout << "**** AssemblyError: Symbol " << sym << " nither defined nor declared with global/extern directive! ****" << endl;
          return -1;
        }
      }
    }
  }
  return 0;
}
bool Assembler::canRepresentOn12Bits(int value)
{
  if (value >= -2048 && value <= 2047)
    return true;
  return false;
}

Assembler::Assembler(string inFileName, string outFileName)
{
  this->idSymbol = 0; // Novo
  this->inputFileName = inFileName;
  this->outputFileName = outFileName;
  this->locationCounter = 0;
  this->currentSectionName = "";
  this->idSection = 0;
  this->currentLine = 1;

  SymbolTable undefinedSymbol;
  undefinedSymbol.idSymbol = idSymbol++;
  undefinedSymbol.nameSymbol = "UNDEFINED";
  undefinedSymbol.idSection = idSection++;
  undefinedSymbol.value = 0;
  undefinedSymbol.typeSymbol = 0;
  undefinedSymbol.bind = 0;
  undefinedSymbol.size = 0;
  undefinedSymbol.defined = 1;
  symbolTable["UNDEFINED"] = undefinedSymbol;
}
int Assembler::readHelperPool()
{

  cout << "Ime\t\t\tOffset\t\t\tImeSekcije\t\t\tStartnaAdresa\t\t\tTip" << endl;
  for (auto a : valuesForPool)
  {
    cout << a.name << "\t\t\t" << a.offset << "\t\t\t" << a.sectionName << "\t\t\t" << a.startAddresInstr << "\t\t\t" << a.type << endl;
  }
  cout << endl;
  cout << endl;

  cout << "Offset\t\t\tSymbolName\t\t\tSectionName\t\t\tAddend\t\t\tTip" << endl;
  for (auto a : relocationTable)
  {
    cout << a.offset << "\t\t\t" << a.symbolName << "\t\t\t" << a.sectionName << "\t\t\t" << a.addend << "t\t\t" << a.type << endl;
  }

  return 0;
}

int Assembler::readRelocations()
{
  cout << endl;
  cout << "Offset\t\t\tSymbolName\t\t\tSectionName\t\t\tAddend\t\t\tTip" << endl;
  for (auto a : relocationTable)
  {
    cout << hex << setw(8) << setfill('0') << a.offset << "\t\t\t" << a.symbolName << "\t\t\t" << a.sectionName << "\t\t\t" << a.addend << "\t\t\t" << a.type << endl;
  }
  cout << endl;
  return 0;
}
vector<string> Assembler::splitString(const std::string &str, char delimiter)
{
  std::vector<std::string> result;
  std::stringstream ss(str);
  std::string item;

  // Izdvajanje delova stringa
  while (std::getline(ss, item, delimiter))
  {
    if (!item.empty())
    { // Dodaje samo ne-prazne delove
      result.push_back(item);
    }
  }

  return result;
}
string Assembler::substringAfterComma(const std::string &str)
{
  // Pronađi poziciju prvog ',' u stringu
  size_t pos = str.find(',');

  // Ako ',' nije pronađen, vrati prazan string
  if (pos == std::string::npos)
  {
    return "";
  }

  // Vraća podstring koji počinje od prvog znaka posle ',' do kraja stringa
  return str.substr(pos + 1);
}
