#include "sdl_utils.h"

#include <stdio.h>
#include <stdlib.h>

// Crée une fenêtre avec un renderer.
// Retourne `NULL` en cas de problème
sdl* sdl_start(int w, int h, char* name) {
	// Pour créer une fenêtre, on veut initialiser la vidéo
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Erreur SDL_Init : %s\n", SDL_GetError());
		return NULL;
	}

	// On crée une fenêtre
	SDL_Window* window = SDL_CreateWindow(name, w, h, 0);
	if (!window) {
		fprintf(stderr, "Erreur SDL_CreateWindow : %s\n", SDL_GetError());
		SDL_Quit();
		return NULL;
	}

	// On crée un renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		fprintf(stderr, "Erreur SDL_CreateRenderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return NULL;
	}

	sdl* utils = malloc(sizeof(sdl));
	if (!utils) {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return NULL;
	}
	utils->window = window;
	utils->renderer = renderer;

	// On affiche full pixel noir
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	// Flush l'écran
	SDL_RenderPresent(renderer);

	return utils;
}

// Sortie douce et gentille d'un `sdl*` créé avec `sdl_start`
// Free `s` aussi
void sdl_exit(sdl* s) {
	if (s->renderer) SDL_DestroyRenderer(s->renderer);
	if (s->window) SDL_DestroyWindow(s->window);
	SDL_Quit();
	free(s);
}

