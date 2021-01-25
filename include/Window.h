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
	void (*handleEvent)(struct Window*, int*, const SDL_Event*);
} Window;

#ifdef __cplusplus
extern "C" {
#endif

Window* createWindow(const char* title, int width, int height, int shown, void (*handleEvent)(Window*, int*, const SDL_Event*));
void destroyWindow(Window* window);

void handleEventDebugWindow(Window* window, int* quit, const SDL_Event* event);
void handleEventMainWindow(Window* window, int* quit, const SDL_Event* event);

uint32_t getWindowIDByEvent(const SDL_Event* event);
void updateMainWindow(Window* window, uint32_t textureID, uint32_t shaderID, uint32_t quadVAO);

#ifdef __cplusplus
}
#endif
#endif