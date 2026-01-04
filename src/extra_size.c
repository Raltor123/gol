#include "extra_size.h"

#include <stdio.h>
#include <stdlib.h>

extra_size* extra_size_create(size_t width) {
	if (width == 0) return NULL;
	extra_size* e = malloc(sizeof(extra_size));
	if (!e) return NULL;

	// Calcule le nombre de size_t nécessaires pour avoir width bits
	e->nb_elts = (width - 1) / (sizeof(size_t) * 8) + 1;

	e->data = calloc(e->nb_elts, sizeof(size_t));
	if (!e->data) {
		free(e);
		return NULL;
	}

	e->witdh = width;

	return e;
}

void extra_size_free(extra_size* e) {
	if (!e) return;
	if (e->data) free(e->data);
	free(e);
}

unsigned char extra_size_get_bit(extra_size* e, size_t x) {
	// Calcule dans quel size_t le bit x est
	// x devient l'index du bit dans la size_t
	size_t pos = 0;
	while (x >= sizeof(size_t) * 8) {
		pos++;
		x -= sizeof(size_t) * 8;
	}

	// Récupère l'index en prenant l'indexage 0 à gauche (bit de poids fort = 0)
	size_t index = 0;
	// Si le nombre total de bits est plus petit que la dernière size_t
	if (pos == e->nb_elts - 1)
		index = (e->witdh - pos * (sizeof(size_t) * 8)) - x - 1;
	else
		index = (sizeof(size_t) * 8) - x - 1;

	return (e->data[pos] >> index) & 1;
}

void extra_size_set_bit(extra_size* e, size_t x) {
	// Calcule dans quel size_t le bit x est
	// x devient l'index du bit dans la size_t
	size_t pos = 0;
	while (x >= sizeof(size_t) * 8) {
		pos++;
		x -= sizeof(size_t) * 8;
	}

	size_t index = 0;
	if (pos == e->nb_elts - 1)
		index = (e->witdh - pos * (sizeof(size_t) * 8)) - x - 1;
	else
		index = (sizeof(size_t) * 8) - x - 1;

	e->data[pos] |= ((size_t)1 << index);
}

void extra_size_unset_bit(extra_size* e, size_t x) {
	// Calcule dans quel size_t le bit x est
	// x devient l'index du bit dans la size_t
	size_t pos = 0;
	while (x >= sizeof(size_t) * 8) {
		pos++;
		x -= sizeof(size_t) * 8;
	}

	// Récupère l'index en prenant l'indexage 0 à gauche (bit de poids fort = 0)
	size_t index = 0;
	// Si le nombre total de bits est plus petit que la dernière size_t
	if (pos == e->nb_elts - 1)
		index = (e->witdh - pos * (sizeof(size_t) * 8)) - x - 1;
	else
		index = (sizeof(size_t) * 8) - x - 1;

	if ((e->data[pos] >> index) & 1)
		e->data[pos] -= ((size_t)1 << index);
}

unsigned char extra_size_mask(extra_size* e, size_t mask, size_t x) {
	// Mask peut être seulement 111 ou 101
	// x = bit central du masque
	size_t pos = 0; // index de la size_t qui contient x
	while (x >= sizeof(size_t) * 8) {
		pos++;
		x -= sizeof(size_t) * 8;
	}
	size_t index = 0;
	if (pos == e->nb_elts - 1)
		index = (e->witdh - pos * (sizeof(size_t) * 8)) - x - 1;
	else
		index = (sizeof(size_t) * 8) - x - 1;

	// res contient la size_t & mask
	size_t res;
	if (index > 0)
		res = mask & (e->data[pos] >> (index - 1));
	else // edge case si l'index est 0
		res = (mask >> 1) & (e->data[pos] >> index);

	// Edges-cases
	// Ici on doit regarder si dans la size_t suivante, le premier bit est à 1
	if (index == 0 && pos < e->nb_elts - 1) {
		res |= ((e->data[pos + 1] >> (8*sizeof(size_t) - 1)) & 1) << 2;
	// Ici on doit regarder si dans la size_t précédente, si elle existe, le dernier bit est à 1
	} else if (index == 63 && pos > 0) {
		res <<= 1;
		res |= e->data[pos - 1] & 1;
	}

	return (res & 1) + ((res >> 1) & 1) + ((res >> 2) & 1);
}

void extra_size_reset(extra_size* e) {
	if (!e) return;
	for (size_t i = 0; i < e->nb_elts; i++)
		e->data[i] ^= e->data[i];
}

