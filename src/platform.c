#include "platform.h"

#include <stdlib.h>

static void panic_and_abort(const char *title, const char *text) {
	fprintf(stderr, "PANIC: %s ... %s\n", title, text);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, text, NULL);
    SDL_Quit();
    exit(1);
}

platform_t *platform_init(const char *title, int window_width, int window_height, int texture_width, int texture_height) {
	platform_t *p = malloc(sizeof(platform_t *));	
	if (p == NULL) {
		panic_and_abort("Failed to initialize platform", "Out of memory");
		return NULL;
	}

	p->window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 32 * 10, 32 * 10, SDL_WINDOW_SHOWN);
	if (!p->window) {
        panic_and_abort("SDL_CreateWindow failed", SDL_GetError());
    }

	p->renderer = SDL_CreateRenderer(p->window, -1, SDL_RENDERER_PRESENTVSYNC);
	if (!p->renderer) {
        panic_and_abort("SDL_CreateRenderer failed", SDL_GetError());
    }

	SDL_RenderSetScale(p->renderer, 10, 10);

	p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 32, 32);
	if (!p->texture) {
        panic_and_abort("SDL_CreateTexture failed", SDL_GetError());
    }

	return p;
}

void platform_free(platform_t *platform) {
	SDL_DestroyWindow(platform->window);
	SDL_DestroyRenderer(platform->renderer);
	SDL_DestroyTexture(platform->texture);
	SDL_Quit();
}
