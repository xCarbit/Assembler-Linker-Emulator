#include<string>
#include<vector>
#include<list>
#include<map>
#include "structs.h"
using namespace std;


class Assembler {

   string inputFileName;
   string outputFileName;

   string currentSectionName;
   int locationCounter;
   int idSymbol;
   int idSection;

   int currentLine;
   std::list<string> orderSections; //Imena sekcija u redosledu navedenom u .s fajlu
   map<string, SymbolTable> symbolTable;
   vector<RelocationTable>  relocationTable;
   map<string, vector<int>> sectionTable;
   
   vector<PoolTable> valuesForPool;
   map<string, vector<Wrapper>> equPool;

   public:
    
    Assembler(string inFileName, string outFileName);
    
    int compile();

    private:

    int createHexFile();
    int createHelperFile();
    int backPatching();
    int processEquPool();
    string getNameOfSectionForId(int idsec);
    
     //Methods for processing st instruction
    int processStMemDir(string arg1, string arg2);
    int processStRegDir(string arg1, string arg2);
    int processStRegInd(string arg1, string arg2);
    int processStRegIndPomLit(string arg1, string arg2, string arg3);
    int processStRegIndPomSym(string arg1, string arg2, string arg3);
    
    //Methods for processing ld instruction

    int porcessLdImm(string arg1, string arg2);
    int processLdRegDir(string arg1, string arg2);
    int processLdMemDir(string arg1, string arg2);
    int processLdRegInd(string arg1, string arg2);
    int processLdRegIndPomLit(string arg1, string arg2, string arg3);
    int processLdRegIndPomSym(string arg1, string arg2, string arg3);

    int stringToDecimalNumber(string);
    int getIdCurrentSection();
    bool canRepresentOn12Bits(int);

    //Methods for processing directives
  
    int processSymbolDirective(string);
    int processGlobalDirective(string);
    int processExternDirective(string);
    int processSectionDirective(string);
    int processSkipDirective(string);
    int processWordDirective(string);
    int processEndDirective(string);
    int processAsciiDirective(string);
    int processEquDirective(string sym, string expr);
    
    //Methods for processing instructions

    int processInstrWithoutOperand(string instr);
    int processInstWithOneOperand(string instr, string arg);
    int processBranchInstruction(string inst, string arg1, string arg2, string arg3);
    int processAritmLocigInstruction(string inst, string arg1, string arg2);
    int processCSRRDInstruction(string arg1, string arg2);
    int processCSRWRInstruction(string arg1, string arg2);

    int readHelperPool();
    int readRelocations();

    vector<string> splitString(const std::string& str, char delimiter = ' ');
    string substringAfterComma(const std::string& str);
};