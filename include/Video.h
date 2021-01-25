#ifndef _VIDEO_H
#define _VIDEO_H

#include <stdint.h>

typedef struct CPU CPU;
typedef struct MMU MMU;

typedef enum VideoMode {
    VIDEO_MODE_HBLANK,
    VIDEO_MODE_VBLANK,
    VIDEO_MODE_OAM,
    VIDEO_MODE_VRAM,
} VideoMode;

#define VIDEO_WIDTH 160
#define VIDEO_HEIGHT 144

#define HLINES 143
#define VLINES 153

const uint16_t videoTimes[4] = { 204, 456, 80, 172 };

typedef struct Video {
    VideoMode mode;
    uint32_t currentClock;

    struct
    {
        union
        {
            uint8_t value;
            struct
            {
                uint8_t lcd_display:1;
                uint8_t window_tilemap_select:1;
                uint8_t window_display:1;
                uint8_t bg_win_tiledata_select:1;
                uint8_t bg_tilemap_select:1;
                uint8_t sprite_size:1;
                uint8_t sprite_display:1;
                uint8_t bg_win_priority:1;
            };
        };
    } *control;

    uint8_t* scroll_y;
    uint8_t* scroll_x;
    uint8_t* line;
    uint8_t* window_y;
    uint8_t* window_x;

    uint32_t colors[4];
    uint8_t palette[4];
    uint8_t tileset[0x200][8][8];

    int canrender;
    uint32_t framebuffer[VIDEO_HEIGHT * VIDEO_WIDTH];

    uint8_t* vram;
    uint8_t* oam;
} Video;

#ifdef __cplusplus
extern "C" {
#endif
Video* createVideo(MMU* mmu);
void destroyVideo(Video* video);

void stepVideo(Video* video, CPU* cpu, MMU* mmu);
void updateMemoryVideo(Video* video, MMU* mmu);
void updateTileVideo(Video* video, uint16_t addr, uint8_t data);

void renderLineVideo(Video* video, MMU* mmu);

#ifdef __cplusplus
}
#endif

#endif