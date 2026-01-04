#include "gol.h"
#include "extra_size.h"

#include <stdio.h>
#include <stdlib.h>

board* board_create(size_t width, size_t height) {
	board* b = malloc(sizeof(board));
	if (!b) return NULL;

	b->data = malloc(height * sizeof(extra_size*));
	if (!b->data) {
		free(b);
		return NULL;
	}

	for (size_t i = 0; i < height; i++) {
		b->data[i] = extra_size_create(width);
		if (!b->data[i]) {
			for (size_t j = 0; j < i; j++)
				extra_size_free(b->data[j]);
			free(b->data);
			free(b);
			return NULL;
		}
	}

	b->width = width;
	b->height = height;
	return b;
}

void board_free(board* b) {
	if (!b) return;
	if (b->data) {
		for (size_t i = 0; i < b->height; i++)
			extra_size_free(b->data[i]);
		free(b->data);
	}
	free(b);
}

unsigned char board_get_bit(board* b, size_t x, size_t y) {
	if (!b) return 0;
	if (!b->data) return 0;
	if (x >= b->width || y >= b->height) return 0;

	return extra_size_get_bit(b->data[y], x);
}

void board_set_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->width || y >= b->height) return;

	extra_size_set_bit(b->data[y], x);
}

void board_unset_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->width || y >= b->height) return;

	extra_size_unset_bit(b->data[y], x);
}

void board_inverse_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->width || y >= b->height) return;

	if (extra_size_get_bit(b->data[y], x))
		extra_size_unset_bit(b->data[y], x);
	else
		extra_size_set_bit(b->data[y], x);
}

unsigned char board_neighbours(board* b, size_t x, size_t y) {
	size_t sum = 0;
	/* Version vraiiiment guez
	sum += board_get_bit(b, x - 1, y - 1);
	sum += board_get_bit(b, x, y - 1);
	sum += board_get_bit(b, x + 1, y - 1);

	sum += board_get_bit(b, x - 1, y);
	sum += board_get_bit(b, x + 1, y);

	sum += board_get_bit(b, x - 1, y + 1);
	sum += board_get_bit(b, x, y + 1);
	sum += board_get_bit(b, x + 1, y + 1);
	*/

	if (y > 0) sum += extra_size_mask(b->data[y - 1], 0b111, x);
	sum += extra_size_mask(b->data[y], 0b101, x);
	if (y < b->height - 1) sum += extra_size_mask(b->data[y + 1], 0b111, x);

	return sum;
}

void board_step(board* b, board* nb) {
	#pragma omp parallel for
	for (size_t y = 0; y < b->height; y++) {
		for (size_t x = 0; x < b->width; x++) {
			size_t nb_neighbours = board_neighbours(b, x, y);
			size_t isAlive = board_get_bit(b, x, y);

			if ((isAlive && nb_neighbours == 2) || nb_neighbours == 3)
				board_set_bit(nb, x, y);
		}
	}

	// Échange les 2 tableaux plutôt que de les copier
	extra_size** tmp = b->data;
	b->data = nb->data;
	nb->data = tmp;

	// Reset le tableau du nouveau plateau
	for (size_t i = 0; i < nb->height; i++)
		extra_size_reset(nb->data[i]);
}

void board_print(board* b) {
	for (size_t y = 0; y < b->height; y++) {
		for (size_t x = 0; x < b->width; x++) {
			if (board_get_bit(b, x, y))
				putchar('1');
			else
				putchar('0');
		}
		putchar('\n');
	}
}

