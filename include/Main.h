#ifndef _MAIN_H
#define _MAIN_H

#include "CPU.h"
#include "Memory.h"

//int DisassembleGB(unsigned char *codebuffer, int pc);

void PrintGBState(CPU *cpu, MMU* mmu);

#endif