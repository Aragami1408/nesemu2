#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include <SDL2/SDL.h>

typedef struct platform platform_t;

platform_t *platform_init(const char *title, int window_width, int window_height, int texture_width, int texture_height);

void platform_free(platform_t *platform);

struct platform {
	SDL_Window *window;
	SDL_GLContext gl_context;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
};

#endif
