#include "args.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int help(void) {
	printf("Usage:\n");
	printf("	./main [-gw|--game-width value] [-gh|--game-height value] [-b|--block-size value] [-p|--padding value] [-nd|--default-pattern] [-t|--threads value]\n");
	printf("	game-width        :	size_t,	default: 64,	maximum width of the board\n");
	printf("	game-height       :	size_t,	default: 64,	maximum height of the board\n");
	printf("	block-size        :	size_t,	default: 10,	size of each cell\n");
	printf("	padding           :	size_t,	default:  1,	padd every cell to make them more distinguishable\n");
	printf("	no-default-pattern:	bool,	default:  t,	remove the basic cell layout at the begining.\n");
	printf("	threads           :	uchar,	default:all,	how many threads to use (doesn't take hyper-threads, only physicals).\n");
	printf("Controls:\n");
	printf("	Space      :	Pause / Play\n");
	printf("	Up arrow   :	Increase play speed\n");
	printf("	Down arrow :	Decrease play speed\n");
	printf("	Right arrow:	Single step\n");
	printf("	U          :	Activate / Desactivate unlimited simulation speed (computer goes brrrrrrrrrrrrrrr)\n");
	printf("	Left click :	(Pause) set cell to live\n");
	printf("	Right click:	(Pause) set cell to death\n");
	return 0;
}

char is_valid(char* c) {
	return (*c) & 1;
}
char is_help(char* c) {
	return ((*c) >> 1) & 1;
}
char is_default_pattern(char* c) {
	return ((*c) >> 2) & 1;
}

/// Récupère le nombre de coeurs physiques du processeur
int get_physical_cores(void) {
	FILE *fp = fopen("/proc/cpuinfo", "r");
	if (!fp) {
		perror("Error opening /proc/cpuinfo");
		return 0;
	}

	int max_core_id = -1;
	char line[256];
	while (fgets(line, sizeof(line), fp)) {
		int core_id = -1;
		if (sscanf(line, "core id\t: %d", &core_id) == 1) {
			if (core_id > max_core_id)
				max_core_id = core_id;
		}
	}

	fclose(fp);
	return max_core_id + 1;
}

args parse_args(int argc, char** argv) {
	args args;
	if (argc < 1) return args;

	args.bools = 0;
	args.game_width = 64;
	args.game_height = 64;
	args.block_size = 10;
	args.padding = 1;
	args.threads = get_physical_cores();
	if (args.threads == 0) return args; // Problem when getting threads

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
			args.bools |= (1 << 1); // set help mode
			args.bools |= 1; // valid
			return args;

		} else if (strcmp(argv[i], "-gw") == 0 || strcmp(argv[i], "--game-width") == 0) {
			if (i + 1 >= argc) return args; // invalid
			args.game_width = strtod(argv[++i], NULL);

		} else if (strcmp(argv[i], "-gh") == 0 || strcmp(argv[i], "--game-height") == 0) {
			if (i + 1 >= argc) return args; // invalid
			args.game_height = strtod(argv[++i], NULL);

		} else if (strcmp(argv[i], "-b") == 0 || strcmp(argv[i], "--block-size") == 0) {
			if (i + 1 >= argc) return args; // invalid
			args.block_size = strtod(argv[++i], NULL);

		} else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--padding") == 0) {
			if (i + 1 >= argc) return args; // invalid
			args.padding = strtod(argv[++i], NULL);

		} else if (strcmp(argv[i], "-nd") == 0 || strcmp(argv[i], "--no-default-pattern") == 0) {
			args.bools |= (1 << 2); // set default pattern

		} else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--threads") == 0) {
			if (i + 1 >= argc) return args; // invalid
			unsigned char t = strtod(argv[++i], NULL);
			if (t < args.threads)
				args.threads = t;

		} else {
			return args;
		}
	}

	// set valid
	args.bools |= 1;

	return args;
}

