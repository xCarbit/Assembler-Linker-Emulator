# Assembler, Linker and Emulator for an Abstract Processor

## About the project

This project is a complete toolchain for a custom 32-bit abstract processor: a two-pass **assembler**, a **linker** and an **emulator**, all written in C++. The goal is to take human-readable assembly source code and carry it all the way to actual execution on a simulated machine, the same way a real `as` → `ld` → CPU pipeline works. The **assembler** takes one `.s` assembly file as input (with directives such as `.global`, `.extern`, `.section`, `.word`, `.skip`, `.ascii`, `.equ`, `.end`) and produces a relocatable object file containing the symbol table, section headers, relocation records and the generated machine code. The **linker** takes one or more object files as input, resolves external symbols, places sections at the requested addresses and produces either a single memory image in hex format (`-hex`) or a new relocatable object file (`-relocatable`). The **emulator** takes that hex image as input, loads it into its simulated memory and executes the instructions one by one, supporting the full instruction set, the stack, subroutine calls, software/hardware interrupts (timer and terminal), and memory-mapped I/O. Its output is the terminal output produced by the emulated program plus the final state of all 16 general-purpose registers, printed when the program executes `halt`.

## Building and running

### Requirements

- `g++` with C++11 support (or newer)
- A POSIX shell (Linux, macOS, or Git Bash / WSL on Windows)

### 1. Build the three tools

Each test directory contains the compile scripts. From inside one of the test directories (for example `tests/nivo-a`):

```bash
cd tests/nivo-a
sh compileAssembler.sh   # builds ./assembler
sh compileLinker.sh      # builds ./linker
sh compileEmulator.sh    # builds ./emulator
```

Equivalently, you can compile by hand:

```bash
g++ src/assembler/main.cpp src/assembler/assembler.cpp -o assembler
g++ src/linker/main.cpp    src/linker/linker.cpp        -o linker
g++ src/emulator/main.cpp  src/emulator/emulator.cpp -pthread -o emulator
```

### 2. Run the toolchain

Assemble every source file into an object file:

```bash
./assembler -o main.o main.s
./assembler -o math.o math.s
```

Link the object files into a memory image, placing sections at chosen addresses:

```bash
./linker -hex \
  -place=my_code@0x40000000 -place=math@0xF0000000 \
  -o program.hex \
  handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o
```

The linker can also produce a relocatable object file instead of a hex image:

```bash
./linker -relocatable -o partial.o main.o math.o
```

Note that `-hex` and `-relocatable` are mutually exclusive, and exactly one of them must be given.

Finally, run the image on the emulator:

```bash
./emulator program.hex
```

The emulator prints whatever the program writes to the terminal, and on `halt` it prints the final processor state:

```
Emulated processor state:

r0=0x00000000    r1=0x00000000    r2=0x00000000    r3=0x00000000
...
```

### 3. Run the provided tests

Three ready-made test programs are included in `tests/nivo-a`, `tests/nivo-b` and `tests/nivo-c`, each with its own `start.sh` that performs the whole assemble → link → emulate sequence:

```bash
cd tests/nivo-a
sh start.sh
```

## Project structure

```
inc/        header files for the assembler, linker and emulator
src/        implementation of the assembler, linker and emulator
tests/      test programs (nivo-a, nivo-b, nivo-c) with build and run scripts
specifikacija.pdf   full specification of the processor and the toolchain
```
