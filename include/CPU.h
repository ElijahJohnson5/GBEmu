#include <stdint.h>

#ifndef _CPU_H
#define _CPU_H

typedef struct ConditionCodes {
    uint8_t z:1;
    uint8_t n:1;
    uint8_t h:1;
    uint8_t c:1;
    uint8_t pad:4;
} ConditionCodes;

typedef struct CPU {
        struct {
        union {
            struct {
                uint8_t f;
                uint8_t a;
            };
            uint16_t af;
        };
    };

    struct {
        union {
            struct {
                uint8_t c;
                uint8_t b;
            };
            uint16_t bc;
        };
    };

    struct {
        union {
            struct {
                uint8_t e;
                uint8_t d;
            };
            uint16_t de;
        };
    };

    struct {
        union {
            struct {
                uint8_t l;
                uint8_t h;
            };
            uint16_t hl;
        };
    };

    uint16_t sp;
    uint16_t pc;
    ConditionCodes cc;
    uint8_t currentClock;
    uint8_t currentOp;
    uint32_t totalClock;

    uint8_t ime;
} CPU;

typedef enum CpuInterruptions {
    CPU_VBLANK = 0b1,
    CPU_LCD = 0b10,
    CPU_TIMER = 0b100,
    CPU_SERIAL = 0b1000,
    CPU_JOYPAD = 0b10000,
} CpuInterruptions;

typedef struct MMU MMU;

#ifdef __cplusplus
extern "C" {
#endif
CPU* createCPU();
void destroyCPU(CPU* cpu);
void resetCPU(CPU* cpu);
void loadGame(CPU* cpu, MMU* mmu);
void stepCPU(CPU* cpu, MMU* mmu);

#ifdef __cplusplus
}
#endif

#endif