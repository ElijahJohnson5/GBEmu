#ifndef _WINDOW_H
#define _WINDOW_H

#include <SDL.h>

typedef struct Video Video;

typedef struct Window {
	SDL_Window* window;
	SDL_GLContext glContext;
	int width;
	int height;
	int shown;
	uint32_t id;
} Window;

#ifdef __cplusplus
extern "C" {
#endif

Window* createWindow(const char* title, int width, int height, int shown);
void destroyWindow(Window* window);

void handleEvents(Window* window, int* quit);
// void updateMainWindow(Window* window, Video* video);

#ifdef __cplusplus
}
#endif
#endif