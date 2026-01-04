#ifndef SDL_BOARD_H
#define SDL_BOARD_H

#include <SDL3/SDL.h>
#include "gol.h"
#include "sdl_utils.h"

void sdl_board_print(sdl*, board*, size_t block_size, size_t padding);

#endif // ! SDL_BOARD_H
