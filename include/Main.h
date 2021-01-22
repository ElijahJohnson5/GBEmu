#ifndef _MAIN_H
#define _MAIN_H

#include <map>

#include "CPU.h"
#include "Memory.h"
#include "Window.h"
#include "Video.h"


//int DisassembleGB(unsigned char *codebuffer, int pc);

typedef struct DisassembledInstruction {
	char* disassembly;
	int pc;
	bool breakpoint;
} DisassembledInstruction;

DisassembledInstruction disassembleInstructions(unsigned char* codebuffer, int *pc);

void PrintGBState(CPU *cpu, MMU* mmu);

void GUIThread(int& quit, CPU* cpu, MMU* mmu, Video* video, DisassembledInstruction* disassembledInstructions, std::map<int, int> pcToIndex);

#endif