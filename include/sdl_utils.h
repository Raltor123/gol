#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL3/SDL.h>
#include <stddef.h>

typedef struct sdl {
	SDL_Window* window;
	SDL_Renderer* renderer;
	size_t block_size;
	size_t padding;
} sdl;

sdl* sdl_start(int width, int height, char* name);
void sdl_exit(sdl* s);

#endif // ! SDL_UTILS_H
