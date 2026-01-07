#include "sdl_board.h"
#include "gol.h"

void draw_block(sdl* s, size_t x, size_t y, size_t block_size) {
	SDL_FRect rect;
	rect.x = x;
	rect.y = y;
	rect.w = block_size;
	rect.h = block_size;
	SDL_RenderFillRect(s->renderer, &rect);
}

void draw_block_padded(sdl* s, size_t x, size_t y, size_t block_size, size_t padding) {
	SDL_FRect rect;
	rect.x = x + padding;
	rect.y = y + padding;
	rect.w = block_size - padding;
	rect.h = block_size - padding;
	SDL_RenderFillRect(s->renderer, &rect);
}

void sdl_board_print(sdl* s, board* b, size_t block_size, size_t padding) {
	if (!s || !s->renderer) return;
	if (!b || !b->data) return;

	SDL_SetRenderDrawColor(s->renderer, 255, 255, 255, 255);
	if (block_size == 1) {
		for (size_t y = 0; y < b->sy; y++) {
			for (size_t x = 0; x < b->sx; x++) {
				if (board_get_bit(b, x, y) == 1)
					SDL_RenderPoint(s->renderer, x, y);
			}
		}
	} else if (block_size <= 2 * padding) {
		for (size_t y = 0; y < b->sy; y++) {
			for (size_t x = 0; x < b->sx; x++) {
				if (board_get_bit(b, x, y) == 1)
					draw_block(s, block_size*x, block_size*y, block_size);
			}
		}
	} else {
		for (size_t y = 0; y < b->sy; y++) {
			for (size_t x = 0; x < b->sx; x++) {
				if (board_get_bit(b, x, y) == 1)
					draw_block_padded(s, block_size*x, block_size*y, block_size, padding);
			}
		}
	}
}

