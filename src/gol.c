#include "gol.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_WIDTH (sizeof(size_t) * 8 - 2)

board* board_create(size_t width, size_t height) {
	board* b = malloc(sizeof(board));
	if (!b) return NULL;

	b->sx = width;
	b->sy = height;
	b->width = ((BLOCK_WIDTH - 1) + width) / BLOCK_WIDTH;

	b->data = calloc((b->sy+ 1) * b->width + 1, sizeof(size_t));
	if (!b->data) {
		free(b);
		return NULL;
	}

	return b;
}

void board_free(board* b) {
	if (!b) return;
	if (b->data) free(b->data);
	free(b);
}

board* board_copy(board* source) {
	board* copy = board_create(source->sx, source->sy);
	if (copy == NULL) return NULL;

	memcpy(copy->data, source->data, ((source->sy+ 1) * source->width + 1) * sizeof(size_t));

	return copy;
}

unsigned char board_get_bit(board* b, size_t x, size_t y) {
	if (!b) return 0;
	if (!b->data) return 0;
	if (x >= b->sx || y >= b->sy) return 0;

	size_t block_index = 1 + y + (b->sy + 1) * (x / BLOCK_WIDTH);
	size_t pos = BLOCK_WIDTH - (x % BLOCK_WIDTH);

	return (b->data[block_index] >> pos) & 1;
}

void board_set_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->sx || y >= b->sy) return;

	size_t block_index = 1 + y + (b->sy + 1) * (x / BLOCK_WIDTH);
	size_t pos = BLOCK_WIDTH - (x % BLOCK_WIDTH);

	b->data[block_index] |= (size_t) 1 << pos;

	// Condition de bordure de gauche à droite
	if (pos == 1 && x < b->sx - 1)
		b->data[block_index + b->sy + 1] |= ((size_t) 1 << (sizeof(size_t) * 8 - 1));
	if (pos == BLOCK_WIDTH && x > 0)
		b->data[block_index - b->sy - 1] |= 1;
}

void board_unset_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->sx || y >= b->sy) return;

	size_t block_index = 1 + y + (b->sy + 1) * (x / BLOCK_WIDTH);
	size_t pos = BLOCK_WIDTH - (x % BLOCK_WIDTH);

	b->data[block_index] &= ~((size_t) 1 << pos);

	// Condition de bordure de gauche à droite
	if (pos == 1 && x < b->sx - 1)
		b->data[block_index + b->sy + 1] &= ~((size_t) 1 << (sizeof(size_t) * 8 - 1));
	if (pos == BLOCK_WIDTH && x > 0)
		b->data[block_index - b->sy - 1] &= ~(size_t) 1;
}

void board_inverse_bit(board* b, size_t x, size_t y) {
	if (!b) return;
	if (!b->data) return;
	if (x >= b->sx || y >= b->sy) return;

	size_t block_index = 1 + y + (b->sy + 1) * (x / BLOCK_WIDTH);
	size_t pos = BLOCK_WIDTH - (x % BLOCK_WIDTH);

	unsigned char v = b->data[block_index] >> pos & 1; // 0 ou 1

	b->data[block_index] &= ~((size_t) 1 << pos);
	b->data[block_index] |= (size_t) v << pos;

	// Condition de bordure de gauche à droite
	if (pos == 1 && x < b->sx - 1) {
		b->data[block_index + b->sy + 1] &= ~((size_t) 1 << (sizeof(size_t) * 8 - 1));
		b->data[block_index + b->sy + 1] |= (size_t) v << (sizeof(size_t) * 8 - 1);
	}
	if (pos == BLOCK_WIDTH && x > 0) {
		b->data[block_index - b->sy - 1] &= ~(size_t) 1;
		b->data[block_index - b->sy - 1] |= ~(size_t) v;
	}
}

void board_step(board* b, board* nb) {
	#pragma omp parallel for
	for (size_t x = 0; x < b->width; x++) {
		// Skip boundary
		for (size_t y = 0; y < b->sy; y++) {
			size_t idx = y + b->sy * x + (x + 1);
			// Count v1
			size_t v = b->data[idx-1] << 1;
			size_t s0 = v;
			size_t s1 = v;

			// Count v2
			v = b->data[idx-1];
			s0 ^= v;
			s1 &= v;

			// Count v3
			v = b->data[idx-1] >> 1;
			size_t buf = s0 & v;
			s0 ^= v;
			s1 |= buf;

			// Count v4
			v = b->data[idx+1] << 1;
			buf = s0 & v;
			size_t of = s1 & buf;
			s0 ^= v;
			s1 |= buf;

			// Count v5
			v = b->data[idx+1];
			buf = s0 & v;
			of |= s1 & buf;
			s0 ^= v;
			s1 |= buf;

			// Count v6
			v = b->data[idx+1] >> 1;
			buf = s0 & v;
			of |= s1 & buf;
			s0 ^= v;
			s1 |= buf;

			// Count v7
			v = b->data[idx] << 1;
			buf = s0 & v;
			of |= s1 & buf;
			s0 ^= v;
			s1 |= buf;

			// Count v8
			v = b->data[idx] >> 1;
			buf = s0 & v;
			of |= s1 & buf;
			s0 ^= v;
			s1 |= buf;

			// Compute final value
			v = b->data[idx];
			size_t new_v = (s0 | v) & s1 & ~of;

			nb->data[idx] = new_v;
		}
	}

	// --- Fill left boundary ---
	// Clear the leftmost bit of each row
	#pragma omp parallel for
	for (size_t y = 0; y < b->sy; y++)
		nb->data[y+1] &= ~(1ul << 63);

	// --- Fill vertical boundaries
	#pragma omp parallel for
	for (size_t x = 0; x < b->width+1; x++)
		nb->data[x * (b->sy + 1)] ^= nb->data[x * (b->sy + 1)];

	// --- Copy the overlapping cells & fill vertical boundary ---
	size_t idx = 0;
	//#pragma omp parallel for
	for (size_t x = 0; x < b->width-1; x++) {
		idx++;
		for (size_t y = 0; y < b->sy; y++) {
			size_t left = nb->data[idx];
			size_t right = nb->data[idx + b->sy+1];

			// Left to right
			right = (right & ~(1ul << 63)) | (((left & (1ul << 1)) << 62));
			// Right to left
			left = (left & (~(1ul << 0))) | ((right >> 62) & 1ul);

			nb->data[idx] = left;
			nb->data[idx + b->sy+1] = right;
			idx++;
		}
	}

	// --- Fill right boundary ---
	// Clear every bit to the right of the simulation
	size_t mask = ~((1ul << (BLOCK_WIDTH - (b->sx - 1) % BLOCK_WIDTH)) - 1ul);
	#pragma omp parallel for
	for (size_t y = 0; y < b->sy; y++)
		nb->data[1 + y + (b->sy+1)*(b->width-1)] &= mask;

	// Switch new_board and board
	size_t* tmp = b->data;
	b->data = nb->data;
	nb->data = tmp;
	// Reset tmp
	#pragma omp parallel for
	for (size_t i = 0; i < (b->sy+ 1) * b->width + 1; i++)
		nb->data[i] ^= nb->data[i];
}

