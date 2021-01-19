#ifndef _MAIN_H
#define _MAIN_H

#include "CPU.h"
#include "Memory.h"


//int DisassembleGB(unsigned char *codebuffer, int pc);

typedef struct DisassembledInstruction {
	char* disassembly;
	int pc;
	bool breakpoint;
} DisassembledInstruction;

DisassembledInstruction disassembleInstructions(unsigned char* codebuffer, int *pc);

void PrintGBState(CPU *cpu, MMU* mmu);

#endif