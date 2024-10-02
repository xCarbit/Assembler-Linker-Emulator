#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include "structs.h"
using namespace std;


class Linker
{


  int numargs;
  const char **arguments;
  string currentFileName;
  string currentSectionName;

  string outputFileName;
  bool hexa;  bool reloc;
  unsigned int firstAvailableAdr;

  map<string,unsigned int> predefinedStartAdr;
  map<string, vector<int>> data;
  map<string,int> sectionsStartAdr;
  
  map<string,SymbolTable> symbolTable;
  list<RelocationTable> relocations;
  map<pair<string,string>,vector<int>> dataOfFilesSections;
  list<Pair> order;
  map<string, int> sectionsSize;
  map<pair<string, string>, int> sectionsOffset; 

  list<string> inputFiles;
  
  
public:
  Linker(int arc, const char *argv[]);
  
  int link();
 
  private:

   int fillExeFile(); //Odredjuje pocetne virtuelne adrese sekcija i kopira njihov sadrzaj u exe fajl tj u data  strukturu
   int findValueOfSymbols();
   int solveRelocations();

   int processSTLine(string symname, string symOrSec, string val, string defOrNot, string sectionName);
   int processRTLine(string _offset, string _type, string _symbolName, string _addend, string _sectionName);
   int processSDLine(string arg);

   int findSizeOfSections();
   int findStartAdress();
   int findSizeOfSumbols();

   int createRelocatableFile();
   int createHexFile();

   int readSectionsAddress();
   int stringToDecimalNumber(string numberStr);
   
};
  