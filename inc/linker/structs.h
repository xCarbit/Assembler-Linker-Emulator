#include <string>
using namespace std;

struct SymbolTable 
{
  string symbolName;
  int type; //0 - sekcija, 1 - simbol
  int value;
  int size;
  int defined; //0 - not 1 - yes
  string sectionName; //Ime sekcije u kojoj je simbol definisan
  string fileName; //Ime fajla u kom je simbol definisan 

  //Polja sectionName i fileName su validna samo ukoliko je defined=1

};

struct RelocationTable 
{
   int offset;
   int type; //uvek ce biti 32
   string symbolName; //ime simbola u odnosu na koji se vrsi relokacija
   int addend;
   string sectionName; //ime sekcije u kojoj treba izvrsiti prepravku
   string fileName;
};

struct Pair 
{
    string filename;
    string sectionname;

};