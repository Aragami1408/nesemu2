#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include <stdio.h>
#include <stdbool.h>

typedef struct platform platform_t;

struct platform {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	bool running;
};

#endif
