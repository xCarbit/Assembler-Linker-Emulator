#include <string>
#include <vector>
#include <map>
using namespace std;

struct Wrapper 
{
   string sign;
   string sym_lit;
   bool processed;
};
struct EQU 
{
   string symDstName; //simobl ciju vrednsot treba izracunati
   string symSrc1Name; //Umanjenik
   string symSrc2Name; //Umanjilac
};
struct  PoolTable {
   
   string name; // ime simbola/literala ciju vrednost treba upisati u bazen
   int type; // 0 - literal , 1 - simbol
   string sectionName; // ime sekcije u koju treba upisati diplacement
   int offset; //Offset do polja u koje treba upisati diplacement 
   int startAddresInstr; //Na kom se offsetu nalazi prvi B instrukcije od pocetka sekcije
   bool RegIndPomSym;
   PoolTable(){RegIndPomSym=false;}
};
struct SymbolTable 
{
          
    int idSymbol;
    string nameSymbol;
    int value;
    int typeSymbol; // 0 - section, 1 - symbol
    int idSection;
    int bind; //0 - local, 1 global
    int size;
    int defined; // 0 - No, 1 - Yes
    int equ; //0 - NO, 1- Yes
    int numEquSyms; 
   //Nedostaje tabela obracanja unapred

  
};

struct RelocationTable 
{

        string sectionName; // where to relocate, current section!
        int offset;          // which byte is the start byte for relocation
        int type;         // type of relocation
        string symbolName;  // which symbol is relocated ! (for local symbol it is the section where it is defined)
        int addend;        
  
};


struct SectionData 
{
   map<string, vector<char>> data;
};