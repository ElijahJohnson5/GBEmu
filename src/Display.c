#include "Display.h"
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

	free(window);
	window = NULL;
}

Display* createDisplay()
{
	Display* display = (Display*)malloc(sizeof(Display));

	if (display == NULL)
	{
		printf("Could not create display, malloc failed\n");
		return NULL;
	}

	display->mainWindow = createWindow("GBEmu", 1280, 720, 1);

	if (display->mainWindow == NULL)
	{
		printf("Could not create main window of display\n");
		free(display);
		return NULL;
	}

	display->debugWindow = createWindow("GBEmu Debug", 1280, 720, 0);

	if (display->debugWindow == NULL)
	{
		printf("Could not create debug window of display\n");
		destroyWindow(display->mainWindow);
		free(display);
		return NULL;
	}

	display->glContext = SDL_GL_CreateContext(display->mainWindow->window);

	if (display->glContext == NULL)
	{
		printf("Could not create GL Context of display\n");
		destroyWindow(display->mainWindow);
		destroyWindow(display->debugWindow);
		free(display);
		return NULL;
	}

	SDL_GL_MakeCurrent(display->mainWindow->window, display->glContext);
	SDL_GL_SetSwapInterval(0);

	return display;
}

void destroyDisplay(Display* display)
{
	if (display->glContext != NULL)
	{
		SDL_GL_DeleteContext(display->glContext);
	}

	if (display->mainWindow != NULL)
	{
		destroyWindow(display->mainWindow);
	}

	if (display->debugWindow != NULL)
	{
		destroyWindow(display->debugWindow);
	}

	free(display);
	display = NULL;
}

void handleEvents(Display* display, int* quit, bool (*imGuiEventProcessor)(const SDL_Event* event))
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		imGuiEventProcessor(&event);
		switch (event.type)
		{
			case SDL_QUIT:
				*quit = 1;
				break;
			case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						if (event.window.windowID == display->mainWindow->id)
						{
							*quit = 1;
						}
						else if (event.window.windowID == display->debugWindow->id && display->debugWindow->shown == 1)
						{
							SDL_HideWindow(display->debugWindow->window);
							display->debugWindow->shown = 0;
						}
						break;
					case SDL_WINDOWEVENT_RESIZED:
						if (event.window.windowID == display->mainWindow->id)
						{
							display->mainWindow->width = event.window.data1;
							display->mainWindow->height = event.window.data2;
						}
						else if (event.window.windowID == display->debugWindow->id)
						{
							display->debugWindow->width = event.window.data1;
							display->debugWindow->height = event.window.data2;
						}
						break;
				}
				break;
		}
	}

	const uint8_t* keysArray = SDL_GetKeyboardState(NULL);

	if (keysArray[SDL_SCANCODE_LCTRL] && keysArray[SDL_SCANCODE_B])
	{
		if (display->debugWindow->shown == 0)
		{
			SDL_ShowWindow(display->debugWindow->window);
			display->debugWindow->shown = 1;
		}
	}
}

void updateMainWindow(Display* display)
{
	SDL_GL_MakeCurrent(display->mainWindow->window, display->glContext);
	glViewport(0, 0, (int)display->mainWindow->width, (int)display->mainWindow->height);
	glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
	glClear(GL_COLOR_BUFFER_BIT);
	SDL_GL_SwapWindow(display->mainWindow->window);
}
