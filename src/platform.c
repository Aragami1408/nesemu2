#include "platform.h"


platform_t *platform_init(const char *title, int width, int height, bool fullscreen) {
	// TODO: Write loggings
	platform_t *p = (platform_t *) malloc(sizeof(platform_t));	
	
	if (p == NULL) {
		return NULL;
	}		

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		return NULL;
	}

	p->window = SDL_CreateWindow(
		title,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_SHOWN
	);

	if (p->window == NULL) {
		return NULL;
	}

	p->renderer = SDL_CreateRenderer(p->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (p->renderer == NULL) {
		return NULL;
	}

	p->texture = SDL_CreateTexture(p->renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_TARGET, 32, 32);

	SDL_RenderSetScale(p->renderer, 10.0, 10.0);

	p->running = true;

	return p;
}

void platform_handle_input(platform_t *p, cpu_t *cpu) {
	if(SDL_PollEvent(&(p->event))) {
		switch(p->event.type) {
			case SDL_QUIT:
				p->running = false;
				break;

			case SDL_KEYDOWN:
				switch(p->event.key.keysym.sym) {
					case SDLK_ESCAPE:
						p->running = false;
						break;

					case SDLK_w: 
						cpu_mem_write(cpu, 0xff, 0x77);
						break;

					case SDLK_s: 
						cpu_mem_write(cpu, 0xff, 0x73);
						break;

					case SDLK_a: 
						cpu_mem_write(cpu, 0xff, 0x61);
						break;

					case SDLK_d: 
						cpu_mem_write(cpu, 0xff, 0x64);
						break;

					default:
						break;

				}
			default:
				break;
		}
	}
}
