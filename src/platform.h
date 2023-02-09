#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdbool.h>

#include "cpu.h"

typedef struct platform platform_t;

struct platform {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Event event;

	bool running;
};

platform_t *platform_init(const char *title, int width, int height, bool fullscreen);
void platform_handle_input(platform_t *p, cpu_t *cpu);



#endif
