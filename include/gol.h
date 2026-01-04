#ifndef GOL_H
#define GOL_H

#include "extra_size.h"
#include <stddef.h>

// Bitmap de taille width * height,
// 0,0 est en haut à gauche.
typedef struct board {
	extra_size** data;
	size_t width;
	size_t height;
} board;

board* board_create(size_t width, size_t height);
void board_free(board*);

// Retourne le bit (0 ou 1) à la position (x, y),
// Return 0 en cas de problème.
unsigned char board_get_bit(board*, size_t x, size_t y);
// Essaye de mettre à 1 le bit en position (x, y).
// Ne fait rien en cas de problème.
void board_set_bit(board*, size_t x, size_t y);
// Essaye de mettre à 0 le bit en position (x, y).
// Ne fait rien en cas de problème.
void board_unset_bit(board*, size_t x, size_t y);
// Inverse le bit en position (x, y)
void board_inverse_bit(board*, size_t x, size_t y);

// Compte le nombre de voisins à 1 du bit en (x, y).
// Ce qui est hors tableau vaut 0.
unsigned char board_neighbours(board*, size_t x, size_t y);
// Calcule le tableau suivant.
// Prend en paramètre un tableau temporaire de la même taille, qui est à 0 et sera remis à 0.
void board_step(board*, board*);

void board_print(board*);

#endif // ! GOL_H
