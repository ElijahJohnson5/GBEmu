#ifndef _INSTRUCTIONS_H
#define _INSTRUCTIONS_H

#include <stdio.h>
#include <stdint.h>

typedef struct CPU CPU;
typedef struct MMU MMU;

typedef struct Instruction {
    const char *disassembly;
    uint8_t operandCount;
    uint8_t ticks;
    void (*execute)(CPU*, MMU*);
} Instruction;

void NOP(CPU* cpu, MMU* mmu);
void JP(CPU* cpu, uint16_t addr, uint8_t offset);
void JR(CPU *cpu, int8_t offset);

extern const Instruction instructions[256];
extern const Instruction prefixInstructions[256];

#endif