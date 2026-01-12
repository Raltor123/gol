#ifndef ARGS_H
#define ARGS_H

#include <stddef.h>

/// Structure qui stocke les arguments donnés par l'utilisateur
typedef struct args {
	char bools; // is_valid, help, no_default_pattern
	size_t game_width;
	size_t game_height;
	size_t block_size;
	size_t padding;
	unsigned char threads;
	size_t perf_test_steps;
} args;

/// Affiche l'aide dans le terminal et retourne 0.
int help(void);

/// Vérifie que `args` est valide en récupérant le booléen associé.
char is_valid(char* c);
/// Vérifie si le flag -h a été donnné dans bools.
char is_help(char* c);
/// Vérifie si le flag no_default_pattern a été donné.
char is_default_pattern(char* c);
/// Retourne un objet args qui contient les arguments du programme.
args parse_args(int argc, char** argv);

#endif // ! ARGS_H

