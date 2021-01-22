#include <stdint.h>

#ifndef _MEMORY_H
#define _MEMORY_H

typedef struct MMU
{
    uint8_t bios[0x100];
    struct {
        union {
            uint8_t addr[0x10000];

            struct {
                uint8_t rom[2][0x4000];
                uint8_t vram[0x2000];
                uint8_t eram[0x2000];
                uint8_t wram[0x2000];
                uint8_t wrams[0x1E00];
                uint8_t oam[0xA0];
                uint8_t empty[0x60];
                uint8_t io[0x40];
                uint8_t ppu[0x40];
                uint8_t zram[0x80];
                uint8_t intenable;
            };
        };
    };

    uint8_t *finishedBios;
    uint16_t lastAddress;
    union {
        uint8_t last8;
        uint16_t last16;
    };
    uint8_t lastFilled;
} MMU;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t readAddr8(MMU* mmu, uint16_t addr);
void writeAddr8(MMU* mmu, uint16_t addr, uint8_t data);

uint16_t readAddr16(MMU* mmu, uint16_t addr);
void writeAddr16(MMU* mmu, uint16_t addr, uint16_t data);

MMU* createMMU();
void destroyMMU(MMU* mmu);

void loadBios(MMU* mmu);

#ifdef __cplusplus
}
#endif

#endif