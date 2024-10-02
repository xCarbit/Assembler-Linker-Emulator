#include "../../inc/emulator/emulator.h"
#include "../../inc/emulator/parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <thread>
#include <sys/select.h>
#include <unistd.h>
#include <termios.h>
#include <csignal>
#include <ctime>
#include <unistd.h>
#include <chrono>
using namespace std;
int Emulator::emulate()
{
  this->fillMemory();
  nextInterrupt = chrono::system_clock::now() + chrono::milliseconds(500);
  while (1)
  {
    
    TimerSimulation();
    readInput();
    if ((TimerInterrupt == true || TerminalInterrupt == true) && !SowInterrupt)
    {
    
      callSoftwareInterrupt();
    }
    
    firstByte = machine.memory[(unsigned int)machine.GPR_registers[15]];
    secondByte = machine.memory[(unsigned int)machine.GPR_registers[15] + 1];
    thirdByte = machine.memory[(unsigned int)machine.GPR_registers[15] + 2];
    forthByte = machine.memory[(unsigned int)machine.GPR_registers[15] + 3];

    machine.GPR_registers[15] += 4; // U trenutku izvrsavanja pc pokazuje na prvu sledecu instrukciju

    if (firstByte[0] == '0')
      break;
    else if (firstByte[0] == '1')
    {
      
      callSoftwareInterrupt();
    }
    else if (firstByte[0] == '2')
    {
      callSubRoutine();
    }
    else if (firstByte[0] == '3')
    {
      callJMP();
    }
    else if (firstByte[0] == '4')
    {
      
      callAtomicSwap();
    }
    else if (firstByte[0] == '5')
    {
      
      callAritmeticOperation();
      
    }
    else if (firstByte[0] == '6')
    {
      
      callLogicalOperation();
    }
    else if (firstByte[0] == '7')
    {
      callShiftOperation();
    }
    else if (firstByte[0] == '8')
    {
      callStoreData();
    }
    else if (firstByte[0] == '9')
    {
      callLoadData();
      
    }
    else
    {
      cout << "**** EmulatorError: unknown operation code! ****" << endl;
    }
  }
  resetTerminal(STDIN_FILENO);
  return 0;
}
int Emulator::readRegisterState()
{
  cout << endl;
  cout << "Emulated processor state:" << endl;
  cout<<endl;
  cout << "r0=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[0] << "\t\t" << "r1=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[1] << "\t\t" << "r2=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[2] << "\t\t" << "r3=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[3] << endl;
  cout << "r4=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[4] << "\t\t" << "r5=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[5] << "\t\t" << "r6=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[6] << "\t\t" << "r7=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[7] << endl;
  cout << "r8=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[8] << "\t\t" << "r9=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[9] << "\t\t" << "r10=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[10] << "\t\t" << "r11=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[11] << endl;
  cout << "r12=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[12] << "\t\t" << "r13=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[13] << "\t\t" << "r14=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[14] << "\t\t" << "r15=0x" << hex << setw(8) << setfill('0') << right << machine.GPR_registers[15] << endl;
  cout << endl;

  return 0;
}
int Emulator::callLoadData()
{

  int address;
  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);
  string strDip = expandDiplacement(forthByte + thirdByte[1]);
  int dip = convertHexStringToDecimal(strDip);
  if (firstByte[1] == '0')
  {
    machine.GPR_registers[a] = machine.CSR_registers[b];
  }
  else if (firstByte[1] == '1')
  {

    machine.GPR_registers[a] = machine.GPR_registers[b] + dip;
  }
  else if (firstByte[1] == '2')
  {
    address = machine.GPR_registers[b] + machine.GPR_registers[c] + dip;
    machine.GPR_registers[a] = readFromMemory(address);
    if (dip == -8)
      SowInterrupt = false;
  }
  else if (firstByte[1] == '3')
  {
    machine.GPR_registers[a] = readFromMemory(machine.GPR_registers[b]);

    machine.GPR_registers[b] += dip;
  }
  else if (firstByte[1] == '4')
  {
    machine.CSR_registers[a] = machine.GPR_registers[b];
  }
  else if (firstByte[1] == '5')
  {
    machine.CSR_registers[a] = machine.CSR_registers[b] + dip;
  }
  else if (firstByte[1] == '6')
  {
    address = machine.GPR_registers[b] + machine.GPR_registers[c] + dip;
    machine.CSR_registers[a] = readFromMemory(address);
  }
  else if (firstByte[1] == '7')

  {

    machine.CSR_registers[a] = readFromMemory(machine.GPR_registers[b]);
    machine.GPR_registers[b] += dip;
  }
  else
  {
    cout << "**** CallStoreData Error! ***" << endl;
    return -1;
  }
  return 0;
}
int Emulator::callStoreData()
{

  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);
  string strDip = expandDiplacement(forthByte + thirdByte[1]);
  int dip = convertHexStringToDecimal(strDip);
  int address;
  if (firstByte[1] == '0')
  {
    address = machine.GPR_registers[a] + machine.GPR_registers[b] + dip;
    writeToMemory(address, machine.GPR_registers[c]);
  }
  else if (firstByte[1] == '2')
  {
    address = machine.GPR_registers[a] + machine.GPR_registers[b] + dip;
    address = readFromMemory(address);
    writeToMemory(address, machine.GPR_registers[c]);
  }
  else if (firstByte[1] == '1')
  {
    machine.GPR_registers[a] = machine.GPR_registers[a] + dip;
    address = machine.GPR_registers[a];
    writeToMemory(address, machine.GPR_registers[c]);
  }
  else
  {
    cout << "**** CallStoreData Error! ***" << endl;
    return -1;
  }
  return 0;
}

int Emulator::callShiftOperation()
{
  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);

  if (firstByte[1] == '0')
    machine.GPR_registers[a] = (machine.GPR_registers[b] << machine.GPR_registers[c]);
  else if (firstByte[1] == '1')
    machine.GPR_registers[a] = (machine.GPR_registers[b] >> machine.GPR_registers[c]);
  else
  {
    cout << "**** CallShiftOperation Error! ***" << endl;
    return -1;
  }
  return 0;
}
int Emulator::callLogicalOperation()
{

  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);

  if (firstByte[1] == 0)
    machine.GPR_registers[a] = ~machine.GPR_registers[b];
  else if (firstByte[1] == 1)
    machine.GPR_registers[a] = machine.GPR_registers[b] & machine.GPR_registers[c];
  else if (firstByte[1] == 2)
    machine.GPR_registers[a] = machine.GPR_registers[b] | machine.GPR_registers[c];
  else if (firstByte[1] == 3)
    machine.GPR_registers[a] = machine.GPR_registers[b] ^ machine.GPR_registers[c];
  else
  {
    cout << "**** CallLogicalOperation Error! ***" << endl;
    return -1;
  }
  return 0;
}
int Emulator::callAritmeticOperation()
{
  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);

  if (firstByte[1] == '0')
    machine.GPR_registers[a] = machine.GPR_registers[b] + machine.GPR_registers[c];
  else if (firstByte[1] == '1')
    machine.GPR_registers[a] = machine.GPR_registers[b] - machine.GPR_registers[c];
  else if (firstByte[1] == '2')
    machine.GPR_registers[a] = machine.GPR_registers[b] * machine.GPR_registers[c];
  else if (firstByte[1] == '3')
    machine.GPR_registers[a] = machine.GPR_registers[b] / machine.GPR_registers[c];
  else
  {
    cout << "**** CallAritmeticOperation Error! ***" << endl;
    return -1;
  }
  return 0;
}
int Emulator::callAtomicSwap()
{

  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);
  int temp = machine.GPR_registers[b];
  machine.GPR_registers[b] = machine.GPR_registers[c];
  machine.GPR_registers[c] = temp;
  return 0;
}
int Emulator::callJMP()
{

  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  int c = convertCharToDecimal(thirdByte[0]);
  string strDip = expandDiplacement(forthByte + thirdByte[1]);
  int dip = convertHexStringToDecimal(strDip);

  int jmpAdr;

  if (firstByte[1] == '0')
    jmpAdr = machine.GPR_registers[a] + dip;
  else if (firstByte[1] == '1' && (machine.GPR_registers[b] == machine.GPR_registers[c]))
    jmpAdr = machine.GPR_registers[a] + dip;
  else if (firstByte[1] == '2' && (machine.GPR_registers[b] != machine.GPR_registers[c]))
    jmpAdr = machine.GPR_registers[a] + dip;
  else if (firstByte[1] == '3' && (machine.GPR_registers[b] > machine.GPR_registers[c]))
    jmpAdr = machine.GPR_registers[a] + dip;
  else if (firstByte[1] == '8')
    jmpAdr = readFromMemory(machine.GPR_registers[a] + dip);
  else if (firstByte[1] == '9' && (machine.GPR_registers[b] == machine.GPR_registers[c]))
    jmpAdr = readFromMemory(machine.GPR_registers[a] + dip);
  else if (firstByte[1] == 'a' && (machine.GPR_registers[b] != machine.GPR_registers[c]))
    jmpAdr = readFromMemory(machine.GPR_registers[a] + dip);
  else if (firstByte[1] == 'b' && (machine.GPR_registers[b] > machine.GPR_registers[c]))
    jmpAdr = readFromMemory(machine.GPR_registers[a] + dip);
  else if (firstByte[1] != '0' && firstByte[1] != '1' && firstByte[1] != '2' && firstByte[1] != '3' && firstByte[1] != '8' &&
           firstByte[1] != '9' && firstByte[1] != 'a' && firstByte[1] != 'b')
  {
    cout << "**** CallJmp Error! ***" << endl;
    return -1;
  }
  else
  {//Nije ispunjen uslov za skok
    return 0;
  }

  machine.GPR_registers[15] = jmpAdr;
  return 0;
}
int Emulator::callSubRoutine()
{

  writeToMemory(machine.GPR_registers[14] - 4, machine.GPR_registers[15]);
  machine.GPR_registers[14] -= 4;
  int jmpadr;

  int a = convertCharToDecimal(secondByte[0]);
  int b = convertCharToDecimal(secondByte[1]);
  string strDip = expandDiplacement(forthByte + thirdByte[1]);
  int dip = convertHexStringToDecimal(strDip);
  if (firstByte[1] == '0')
    jmpadr = machine.GPR_registers[a] + machine.GPR_registers[b] + dip;
  else if (firstByte[1] == '1')
    jmpadr = readFromMemory(machine.GPR_registers[a] + machine.GPR_registers[b] + dip);
  else
  {
    cout << "**** CallSubRoutine Error! ***" << endl;
    return -1;
  }
  machine.GPR_registers[15] = jmpadr;

  return 0;
}
int Emulator::callSoftwareInterrupt()
{
  SowInterrupt = true;
  writeToMemory((unsigned int)machine.GPR_registers[14] - 4, machine.CSR_registers[0]);
  writeToMemory((unsigned int)machine.GPR_registers[14] - 8, machine.GPR_registers[15]);
  machine.GPR_registers[14] -= 8;
  machine.CSR_registers[0] = machine.CSR_registers[0] | (0x4);
  if (TerminalInterrupt)
  {
    machine.CSR_registers[2] = 3;
    TerminalInterrupt = false;
    TimerInterrupt = false;
  }
  else if (TimerInterrupt)
  {
    machine.CSR_registers[2] = 2;
    TimerInterrupt = false;
    TerminalInterrupt = false;
  }

  else
    machine.CSR_registers[2] = 4;
  machine.GPR_registers[15] = machine.CSR_registers[1];
  return 0;
}
int Emulator::fillMemory()
{
  ifstream inFile("../../src/emulator/Helper" + inputFileName);
  if (!inFile.is_open())
  {
    cout << "**** Fail to open .hex file ****" << endl;
    return -1;
  }

  for (auto &it : machine.memory)
    it.second = "00000000"; 

  string line;
  smatch matches;
  while (std::getline(inFile, line))
  {
    if (regex_search(line, matches, rg_8MemoryLocation))
    {
      string currByte;
      uint32_t address = stoul(matches[1].str(), nullptr, 16);
      for (int i = 2; i <= 9; i++)
      {
        currByte = matches[i].str();
        if (currByte != "xx")
        {
          machine.memory[address] = currByte;
          address++;
        }
      }
    }
    else
    {

      cout << "**** Nije pronadjen nijedan Regex! ****" << endl;
    }
  }


  return 0;
}
int Emulator::stringToDecimalNumber(string numberStr)
{
  bool isHex = false;
  if (numberStr.size() > 2 && numberStr.substr(0, 2) == "0x")
  {
    isHex = true;
  }
  if (isHex)
  {
    std::string hexStr = numberStr.substr(2);
    return std::stoi(numberStr, nullptr, 16);
  }
  else
  {
    std::istringstream iss(numberStr);
    int decimalValue;
    iss >> decimalValue;
    return decimalValue;
  }
}
int Emulator::readMemory()
{
  for (auto it : machine.memory)
  {
    cout << hex << static_cast<int>(it.first) << ": " << (it.second);
  }
  return 0;
}
string Emulator::convertDecimalToHexString(int number)
{

  char heksa[20];
  std::sprintf(heksa, "%08X", number);
  return heksa;
}
int Emulator::convertHexStringToDecimal(string number)
{
  if (number == " " || number == "  ")
  {
    cout << "Vaild argument " << endl;
    return -1;
  }
  return static_cast<int>(stol(number, nullptr, 16)); // Greska
}

int Emulator::writeToMemory(int address, int value)
{

   
  string strVal = convertDecimalToHexString(value);
  string s = convertDecimalToHexString(address);
  machine.memory[address] = strVal.substr(6, 2); 
  machine.memory[address + 1] = strVal.substr(4, 2);
  machine.memory[address + 2] = strVal.substr(2, 2);
  machine.memory[address + 3] = strVal.substr(0, 2);
  if (convertDecimalToHexString(address) == "FFFFFF00")
    cout << static_cast<char>(stoi(strVal.substr(6, 2), nullptr, 16)) << std::flush;

  return 0;
}
int Emulator::readFromMemory(int address)
{
  string str = machine.memory[address + 3] + machine.memory[address + 2] + machine.memory[address + 1] + machine.memory[address];
  int ret = convertHexStringToDecimal(str);
  return convertHexStringToDecimal(str);
}
int Emulator::convertCharToDecimal(char c)
{
  if (c == 'f' || c == 'F')
    return 15;
  else if (c == 'e' || c == 'E')
    return 14;
  else if (c == 'd' || c == 'D')
    return 13;
  else if (c == 'c' || c == 'C')
    return 12;
  else if (c == 'b' || c == 'B')
    return 11;
  else if (c == 'a' || c == 'A')
    return 10;
  else if (c == '9')
    return 9;
  else if (c == '8')
    return 8;
  else if (c == '7')
    return 7;
  else if (c == '6')
    return 6;
  else if (c == '5')
    return 5;
  else if (c == '4')
    return 4;
  else if (c == '3')
    return 3;
  else if (c == '2')
    return 2;
  else if (c == '1')
    return 1;
  else
    return 0;
}
string Emulator::expandDiplacement(string input)
{

  if (input[0] == 'f' || input[0] == 'e' || input[0] == 'd' || input[0] == 'c' || input[0] == 'b' || input[0] == 'a' || input[0] == '9' || input[0] == '8' || input[0] == 'F' || input[0] == 'E' || input[0] == 'D' || input[0] == 'C' || input[0] == 'B' || input[0] == 'A')
  {

    return "fffff" + input;
  }
  else
  {

    return "00000" + input;
  }
}
Emulator::Emulator(string _inputFileName) : inputFileName(_inputFileName)
{
  for (int i = 0; i < 16; i++)
    machine.GPR_registers[i] = 0;
  machine.CSR_registers[0] = 0;
  machine.CSR_registers[1] = 0;
  machine.CSR_registers[2] = 0;
  machine.GPR_registers[15] = (stoull("40000000", nullptr, 16)); // Ispravka
  TerminalInterrupt = false;
  TimerInterrupt = false;
  readyTermOut = false;
  SowInterrupt = false;
  firstTime=true;
}

void Emulator::setNonBlockingMode(int fd)
{
  struct termios tty;
  if (tcgetattr(fd, &tty) != 0)
  {
    perror("tcgetattr");
    exit(1);
  }
  tty.c_lflag &= ~(ICANON | ECHO); // Isključi canonical mode i echo
  if (tcsetattr(fd, TCSANOW, &tty) != 0)
  {
    perror("tcsetattr");
    exit(1);
  }
}
void Emulator::readInput()
{
  setNonBlockingMode(STDIN_FILENO); // Postavi terminal u neblokirajući režim

  char ch;

  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);

  // Koristi select() za čekanje na ulaz
  struct timeval timeout = {0, 1}; // Koliko dugo ceka na unos karaktera sa teastture
  int activity = select(STDIN_FILENO + 1, &readfds, nullptr, nullptr, &timeout);

  if (activity == -1)
  {
    perror("select()");
    return;
  }

  if (FD_ISSET(STDIN_FILENO, &readfds))
  {
    if (read(STDIN_FILENO, &ch, 1) == 1)
    {
      // std::cout << "You entered: " << ch << std::endl;
      int ascicode = static_cast<int>(ch);
      writeToMemory(4294967044, ascicode);
      TerminalInterrupt = true;
      // Na primer, izlaz iz petlje nakon što se unese 'q'
      if (ch == 'q')
      {
        return;
      }
    }
  }

  // Vratiti terminal u blokirajući režim
  resetTerminal(STDIN_FILENO);
}
void Emulator::resetTerminal(int fd)
{
  struct termios tty;
  if (tcgetattr(fd, &tty) != 0)
  {
    perror("tcgetattr");
    exit(1);
  }
  tty.c_lflag |= (ICANON | ECHO); // Ponovno uključi canonical mode i echo
  if (tcsetattr(fd, TCSANOW, &tty) != 0)
  {
    perror("tcsetattr");
    exit(1);
  }
}
void Emulator::TimerSimulation()
{
  if (firstTime==true)
  {
    writeToMemory(4294967056, 0);
    firstTime = false;
  }
  if (chrono::system_clock::now() >= nextInterrupt)
  {
    int intterruptPeriod = readFromMemory(4294967056);
    if (intterruptPeriod == 0)
      nextInterrupt += chrono::milliseconds(500);
    else if (intterruptPeriod == 1)
      nextInterrupt += chrono::milliseconds(1000);
    else if (intterruptPeriod == 2)
      nextInterrupt += chrono::milliseconds(1500);
    else if (intterruptPeriod == 3)
      nextInterrupt += chrono::milliseconds(2000);
    else if (intterruptPeriod == 4)
      nextInterrupt += chrono::milliseconds(5000);
    else if (intterruptPeriod == 5)
     nextInterrupt += chrono::milliseconds(10000);
    else if (intterruptPeriod == 6)
       nextInterrupt += chrono::milliseconds(30000);
    else if (intterruptPeriod == 7)
      nextInterrupt += chrono::milliseconds(60000);
    else
    {
      cout << "****Error: Ivalid period of timer interrupt!****" << endl;
      return;
    }
    TimerInterrupt = true;
    
  }
}
