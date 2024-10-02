#include "../../inc/emulator/emulator.h"
#include<iostream>
#include <sstream>
#include <iomanip>
using namespace std;
int main(int argn, const char* argv[]) {

  Emulator emulator(argv[1]);
  
  emulator.fillMemory();
  emulator.emulate();
  emulator.readRegisterState();


  return 0;
}