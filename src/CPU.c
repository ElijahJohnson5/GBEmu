#include "CPU.h"
#include <stdlib.h>
#include <string.h>
#include "Memory.h"
#include "Instructions.h"

CPU* createCPU()
{
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    return cpu;
}

void destroyCPU(CPU* cpu)
{
    free(cpu);
    cpu = NULL;
}

void resetCPU(CPU* cpu)
{
    memset(cpu, 0, sizeof(CPU));
}

void loadGame(CPU* cpu, MMU* mmu)
{
    *mmu->finishedBios = 1;

    cpu->pc = 0x0100;
    cpu->sp = 0xFFFE;
    cpu->af = 0x01B0;
    cpu->bc = 0x0013;
    cpu->de = 0x00D8;
    cpu->hl = 0x014D;

    memset(mmu->vram, 0, sizeof(mmu->vram));
}

void stepCPU(CPU* cpu, MMU* mmu)
{
    uint8_t op = readAddr8(mmu, cpu->pc);
    cpu->currentOp = op;

    Instruction current = instructions[op];
    cpu->pc++;
    if (op == 0xCB)
    {
        op = readAddr8(mmu, cpu->pc);
        current = prefixInstructions[op];
        cpu->currentOp = op;
        cpu->pc++;
    }

    if (!current.execute)
    {
        //DEBUG opcode error
        return;
    }

    current.execute(cpu, mmu);
    cpu->currentClock = current.ticks;
    cpu->pc += current.operandCount;

    cpu->totalClock += cpu->currentClock;
}