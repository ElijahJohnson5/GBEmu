#include "Window.h"
#include "glad.h"

Window *createWindow(const char* title, int width, int height, int shown)
{
	Window* window = (Window*)malloc(sizeof(Window));

	if (window == NULL)
	{
		printf("Could not create window: %s, malloc failed\n", title);
		return NULL;
	}

	SDL_WindowFlags windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	windowFlags |= shown ? 0 : SDL_WINDOW_HIDDEN;
	window->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, windowFlags);

	if (window->window == NULL)
	{
		printf("Could not create window: %s, window creationg failed\n", title);
		free(window);
		return NULL;
	}

	window->glContext = SDL_GL_CreateContext(window->window);

	if (window->glContext == NULL)
	{
		printf("Could not create GL Context of display\n");
		SDL_DestroyWindow(window->window);
		free(window);
		return NULL;
	}

	window->shown = shown;
	window->width = width;
	window->height = height;

	window->id = SDL_GetWindowID(window->window);

	return window;
}

void destroyWindow(Window* window)
{
	if (window->window != NULL)
	{
		SDL_DestroyWindow(window->window);
	}

	if (window->glContext != NULL)
	{
		SDL_GL_DeleteContext(window->glContext);
	}

	free(window);
	window = NULL;
}

void handleEvents(Window* window, int* quit)
{


	//const uint8_t* keysArray = SDL_GetKeyboardState(NULL);

	//if (keysArray[SDL_SCANCODE_LCTRL] && keysArray[SDL_SCANCODE_B])
	//{
	//	if (display->debugWindow->shown == 0)
	//	{
	//		SDL_ShowWindow(display->debugWindow->window);
	//		display->debugWindow->shown = 1;
	//	}
	//}
}

void updateMainWindow(Window* window)
{
	SDL_GL_MakeCurrent(window->window, window->glContext);
	glViewport(0, 0, (int)window->width, (int)window->height);
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(window->window);
}
