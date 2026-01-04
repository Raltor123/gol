#ifndef EXTRA_SIZE_H
#define EXTRA_SIZE_H

#include <stddef.h>

typedef struct extra_size {
	size_t* data;
	size_t nb_elts;
	size_t witdh;
} extra_size;

extra_size* extra_size_create(size_t width);
void extra_size_free(extra_size*);

unsigned char extra_size_get_bit(extra_size*, size_t x);
void extra_size_set_bit(extra_size*, size_t x);
void extra_size_unset_bit(extra_size*, size_t x);

unsigned char extra_size_mask(extra_size*, size_t mask, size_t x);
void extra_size_reset(extra_size*);

#endif // ! EXTRA_SIZE_H

