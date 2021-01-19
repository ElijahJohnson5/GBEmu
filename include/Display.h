#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <SDL.h>

#include <stdbool.h>

typedef struct Window {
	SDL_Window* window;
	int width;
	int height;
	int shown;
	uint32_t id;
} Window;

typedef struct Display {
	Window* mainWindow;
	Window* debugWindow;
	SDL_GLContext glContext;
} Display;


#ifdef __cplusplus
extern "C" {
#endif

Window* createWindow(const char* title, int width, int height, int shown);
void destroyWindow(Window* window);

Display* createDisplay();
void destroyDisplay(Display* display);

void handleEvents(Display* display, int* quit, bool (*imGuiEventProcessor)(const SDL_Event *event));
void updateMainWindow(Display* display);

#ifdef __cplusplus
}
#endif
#endif