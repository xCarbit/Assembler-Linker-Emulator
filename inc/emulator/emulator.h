#pragma once
#include "structs.h"
#include <string>
#include <atomic>
#include <ctime>
#include <chrono>

enum RegisterType {GRP, CSR};
class Emulator
{
public:
  struct Machine machine;
  private:
  bool SowInterrupt;
  string inputFileName;
  bool readyTermOut;
  bool TerminalInterrupt;
  bool TimerInterrupt;
  int copystatus;
  bool firstTime;
std::chrono::time_point<std::chrono::system_clock> nextInterrupt=chrono::system_clock::now();

  string firstByte; string secondByte; string thirdByte; string forthByte;

public:
  Emulator(string _inputFileName);
  int emulate();
  int readRegisterState();
  int fillMemory();

  private:

  int readMemory();

  int callSoftwareInterrupt();
  int callSubRoutine();
  int callJMP();
  int callAtomicSwap();
  int callAritmeticOperation();
  int callLogicalOperation();
  int callShiftOperation();
  int callStoreData();
  int callLoadData();


  int writeToMemory(int address, int value);
  int readFromMemory(int address);

  string convertDecimalToHexString(int number);
  int convertHexStringToDecimal(string number);
  int stringToDecimalNumber(string);
  int convertCharToDecimal(char c);
  string expandDiplacement(string input);

  void setNonBlockingMode(int fd);
  void readInput(); 
  void resetTerminal(int fd);
  void TimerSimulation();

};