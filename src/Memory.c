#include "Memory.h"
#include <stdlib.h>
#include <string.h>

static uint8_t BIOS[0x100] = {
	0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
	0x11, 0x3E, 0x80, 0x32, 0xE2, 0x0C, 0x3E, 0xF3, 0xE2, 0x32, 0x3E, 0x77, 0x77, 0x3E, 0xFC, 0xE0,
	0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
	0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
	0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
	0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
	0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
	0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
	0x7B, 0xE2, 0x0C, 0x3E, 0x87, 0xF2, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
	0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
	0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B,
	0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
	0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC,
	0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E, 0x3c, 0x42, 0xB9, 0xA5, 0xB9, 0xA5, 0x42, 0x4C,
	0x21, 0x04, 0x01, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x20,
	0xF5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x20, 0xFE, 0x3E, 0x01, 0xE0, 0x50 
};

uint8_t readAddr8(MMU* mmu, uint16_t addr) 
{
    if (!(*mmu->finishedBios) && addr >= 0x00 && addr <= 0xFF)
        return mmu->bios[addr];

    switch ((addr & 0xF000) >> 12)
    {
        case 0x0: case 0x1: case 0x2: case 0x3:
            return mmu->rom[0][addr];
        case 0x4: case 0x5: case 0x6: case 0x7:
            return mmu->rom[1][addr - 0x4000];
        
        case 0x8:
        case 0x9:
            return mmu->vram[addr - 0x8000];
        
        case 0xA:
        case 0xB:
            return mmu->eram[addr - 0xA000];
        
        case 0xC:
        case 0xD:
            return mmu->wram[addr - 0xC000];

        case 0xE:
            return mmu->wram[addr - 0xC000];

        case 0xF:
            switch (addr & 0x0F00)
            {
                case 0x000: case 0x100: case 0x200: case 0x300:
                case 0x400: case 0x500: case 0x600: case 0x700:
                case 0x800: case 0x900: case 0xA00: case 0xB00:
                case 0xC00: case 0xD00:
                    return mmu->wram[addr - 0x1FFF];
                case 0xE00:
                    if (addr < 0xFEA0)
                    {
                        return mmu->oam[addr & 0xFF];
                    }
                    return 0;
                case 0xF00:
                    if (addr == 0xFFFF)
                    {
                        return mmu->intenable;
                    }
                    else 
                    {
                        switch (addr & 0x00F0)
                        {
                            case 0x00:
                                return mmu->io[addr & 0xFF];
                            case 0x40: case 0x50: case 0x60: case 0x70:
                                return mmu->ppu[addr - 0xFF40];
                            case 0x80: case 0x90: case 0xA0: case 0xB0: case 0xC0: case 0xD0: case 0xE0:
                            case 0xF0:
                                return mmu->zram[addr & 0x7F];
                        }
                    }
            }
    }

    return 0;
}

void writeAddr8(MMU* mmu, uint16_t addr, uint8_t data)
{
    if (!(*mmu->finishedBios) && addr >= 0x00 && addr <= 0xFF)
        return;

    switch ((addr & 0xF000) >> 12)
    {
        case 0x0: case 0x1: case 0x2: case 0x3:
        case 0x4: case 0x5: case 0x6: case 0x7:
            return;
        
        case 0x8:
        case 0x9:
            mmu->vram[addr - 0x8000] = data;
            return;
        
        case 0xA:
        case 0xB:
            mmu->eram[addr - 0xA000] = data;
            return;
        
        case 0xC:
        case 0xD:
            mmu->wram[addr - 0xC000] = data;
            return;

        case 0xE:
            mmu->wram[addr - 0xC000] = data;
            return;

        case 0xF:
            switch (addr & 0x0F00)
            {
                case 0x000: case 0x100: case 0x200: case 0x300:
                case 0x400: case 0x500: case 0x600: case 0x700:
                case 0x800: case 0x900: case 0xA00: case 0xB00:
                case 0xC00: case 0xD00:
                    mmu->wram[addr - 0x1FFF] = data;
                    return;
                
                case 0xE00:
                    if (addr < 0xFEA0)
                    {
                        mmu->oam[addr & 0xFF] = data;
                        return;
                    }
                    return;
                case 0xF00:
                    if (addr == 0xFFFF)
                    {
                        mmu->intenable = data;
                        return;
                    }
                    else 
                    {
                        switch (addr & 0x00F0)
                        {
                            case 0x00:
                                mmu->io[addr & 0xFF] = data;
                                return;
                            case 0x40: case 0x50: case 0x60: case 0x70:
                                mmu->ppu[addr - 0xFF40] = data;
                                return;
                            case 0x80: case 0x90: case 0xA0: case 0xB0: case 0xC0: case 0xD0: case 0xE0:
                            case 0xF0:
                                mmu->zram[addr & 0x7F] = data;
                                return;
                        }
                    }
            }
    }
}

uint16_t readAddr16(MMU* mmu, uint16_t addr)
{
    return (readAddr8(mmu, addr) | (readAddr8(mmu, addr + 1) << 8));
}

void writeAddr16(MMU* mmu, uint16_t addr, uint16_t data)
{
    writeAddr8(mmu, addr, data & 0xFF);
    writeAddr8(mmu, addr + 1, data >> 8);
}

MMU* createMMU()
{
    MMU* mmu = (MMU*)malloc(sizeof(MMU));

    if (mmu == NULL)
    {
        printf("Could not create MMU, malloc failed\n");
        return NULL;
    }

    mmu->finishedBios = mmu->addr + 0xFF50;
    return mmu;
}

void destroyMMU(MMU* mmu)
{
    free(mmu);
    mmu = NULL;
}

void loadBios(MMU* mmu)
{
    memcpy(mmu->bios, BIOS, sizeof(BIOS));
    *mmu->finishedBios = 0;
}