#ifndef GOL_H
#define GOL_H

#include <stddef.h>

// Bitmap de taille width * height,
// 0,0 est en haut à gauche.
typedef struct board {
	size_t* data;
	size_t width;
	size_t sx;
	size_t sy;
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

// Calcule le tableau suivant.
// Prend en paramètre un tableau temporaire de la même taille, qui est à 0 et sera remis à 0.
void board_step(board*, board*);


#endif // ! GOL_H
