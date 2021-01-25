#include "Window.h"
#include "glad.h"

Window *createWindow(const char* title, int width, int height, int shown, void (*handleEvent)(Window*, int*, const SDL_Event*))
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
	window->handleEvent = handleEvent;

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

void handleEventDebugWindow(Window* window, int* quit, const SDL_Event* event)
{
		switch (event->type)
		{
			case SDL_WINDOWEVENT:
				switch (event->window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						if (window->shown)
						{
							window->shown = 0;
							SDL_HideWindow(window->window);
						}
						break;
				}
				break;
			case SDL_WINDOWEVENT_RESIZED:
				window->width = event->window.data1;
				window->height = event->window.data2;
				break;
		}
}

void handleEventMainWindow(Window* window, int* quit, const SDL_Event* event)
{
		switch (event->type)
		{
			case SDL_WINDOWEVENT:
				switch (event->window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
						*quit = 1;
						break;
				}
				break;
			case SDL_WINDOWEVENT_RESIZED:
				window->width = event->window.data1;
				window->height = event->window.data2;
				break;
		}
}

void updateMainWindow(Window* window, uint32_t textureID, uint32_t shaderID, uint32_t quadVAO)
{
	SDL_GL_MakeCurrent(window->window, window->glContext);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glClear(GL_COLOR_BUFFER_BIT);


	glUseProgram(shaderID);
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindTexture(GL_TEXTURE_2D, 0);
	SDL_GL_SwapWindow(window->window);
}
