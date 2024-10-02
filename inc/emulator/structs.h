#pragma once


#include <map>
using namespace std;
struct Machine {
map<uint32_t, string> memory;
 int GPR_registers[16];
 int CSR_registers[3];  //0 - status, 1 - handler, 2 - cause
  
};