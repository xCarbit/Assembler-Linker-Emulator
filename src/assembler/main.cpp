#include "../../inc/assembler/assembler.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

int main(int argc, const char* argv[]) 
{

   if(argc<4) 
    {
      cout<<"Fail to start assembling"; return -1;
    }

   Assembler assm(argv[3], argv[2]);
   assm.compile();

   return 0;
}