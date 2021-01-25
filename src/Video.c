#include "Video.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Memory.h"
#include "CPU.h"

Video* createVideo(MMU* mmu)
{
    Video* video = (Video*)malloc(sizeof(Video));
    video->vram = mmu->vram;
    video->oam = mmu->oam;

    video->control = mmu->ppu;

    video->mode = VIDEO_MODE_OAM;
    video->currentClock = 0;

    video->scroll_y = mmu->ppu + 0x02;
    video->scroll_x = mmu->ppu + 0x03;
    video->line = mmu->ppu + 0x04;
    video->window_y = mmu->ppu + 0x0A;
    video->window_x = mmu->ppu + 0x0B;

    *(video->scroll_x) = 0;
    *(video->scroll_y) = 0;
    *(video->line) = 0;

    video->colors[0] = 0xFFFFFFFF;
    video->colors[1] = 0xC0C0C0FF;
    video->colors[2] = 0x606060FF;
    video->colors[3] = 0x000000FF;

    memset(video->framebuffer, 0, VIDEO_HEIGHT * VIDEO_WIDTH * sizeof(uint32_t));

    memset(video->tileset, 0, sizeof(video->tileset));

    return video;
}

void destroyVideo(Video* video)
{
    free(video);
    video = NULL;
}

void updateMemoryVideo(Video* video, MMU* mmu)
{
    if (!mmu->lastFilled) return;

    if (mmu->lastAddress == 0xFF47)
    {
        // Update palette
        video->palette[3] = (mmu->last8 & 0xc0) >> 6;
        video->palette[2] = (mmu->last8 & 0x30) >> 4;
        video->palette[1] = (mmu->last8 & 0x0c) >> 2;
        video->palette[0] = (mmu->last8 & 0x03);
        mmu->lastFilled = 0;
    }
    else if (mmu->lastAddress >= 0x8000 && mmu->lastAddress < 0xA000)
    {
        // Update tileset
        updateTileVideo(video, mmu->lastAddress, mmu->last8);
        mmu->lastFilled = 0;
    }
}

void updateTileVideo(Video* video, uint16_t addr, uint8_t data)
{
    uint16_t vaddr = addr & 0x1FFE;
    uint16_t index = vaddr >> 4;
    uint8_t y = (vaddr >> 1) & 7;

    uint8_t v = data;
    uint8_t v1 = video->vram[vaddr + 1];

    uint8_t sx, x;

    for (x = 0; x < 8; x++)
    {
        sx = 1 << (7 - x);
        video->tileset[index][y][x] = (v & sx ? 1 : 0) + (v1 & sx ? 2 : 0);
    }
}

void stepVideo(Video* video, CPU* cpu, MMU* mmu)
{
    updateMemoryVideo(video, mmu);

    video->currentClock += cpu->currentClock;

    switch (video->mode)
    {
        case VIDEO_MODE_HBLANK:
            if (video->currentClock >= videoTimes[video->mode])
            {
                video->currentClock %= videoTimes[video->mode];
                *(video->line) = *(video->line) + 1;

                if (*(video->line) == HLINES)
                {
                    video->mode = VIDEO_MODE_VBLANK;
                    video->canrender = 1;
                    (*(mmu->addr + 0xFF00)) |= CPU_VBLANK;
                }
                else 
                {
                    video->mode = VIDEO_MODE_OAM;
                }
            }
            break;
        case VIDEO_MODE_VBLANK:
            if (video->currentClock >= videoTimes[video->mode])
            {
                video->currentClock %= videoTimes[video->mode];
                *(video->line) = *(video->line) + 1;
                
                if (*(video->line) >= VLINES)
                {
                    video->mode = VIDEO_MODE_OAM;
                    *video->line = 0;
                }
            }
            break;
        case VIDEO_MODE_OAM:
            if (video->currentClock >= videoTimes[video->mode])
            {
                video->currentClock %= videoTimes[video->mode];
                video->mode = VIDEO_MODE_VRAM;
            }
            break;
        case VIDEO_MODE_VRAM:
            if (video->currentClock >= videoTimes[video->mode])
            {
                video->currentClock %= videoTimes[video->mode];
                video->mode = VIDEO_MODE_HBLANK;

                renderLineVideo(video, mmu);
            }
            break;
    }
}

void renderLineVideo(Video* video, MMU* mmu)
{
    uint16_t mapoffset = video->control->bg_win_tiledata_select ? 0x1C00 : 0x1800;
    mapoffset += (((*video->line + *video->scroll_y) & 255) >> 3) << 5;

    uint8_t lineoffset = (*video->scroll_x >> 3);

    uint8_t y = (*video->line + *video->scroll_y) & 7;
    
    uint8_t x;
    int8_t tile;
    uint32_t color;

    for (x = 0; x < VIDEO_WIDTH; x++)
    {
        tile = (int8_t)video->vram[mapoffset + lineoffset + (x / 8)];
        if (video->control->bg_tilemap_select && tile < 128) tile += 0x100;

        color = video->colors[video->palette[video->tileset[tile][y][x % 8]]];
        video->framebuffer[*video->line * VIDEO_WIDTH + x] = color;
    }
}