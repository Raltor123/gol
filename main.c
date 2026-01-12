#include <omp.h>
#include <SDL3/SDL.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "args.h"
#include "gol.h"
#include "sdl_board.h"
#include "sdl_utils.h"
#include "vector2.h"

typedef struct game_state {
	board* board;
	board* board_next_step;
	bool* previous_key_states;
	bool playing;
	bool unlimited_simulation; // Dans ce mode on execute les steps le plus vite possible
	double timer;
	double simulation_speed; // Temps réel entre 2 actualisations en mode play
	vector2* last_mouse_position;
	size_t width; // Largeur du plateau
	size_t height; // Hauteur du plateau
	float frame_rate; // Nombre d'image par seconde max
} game_state;

// Flag pour quitter l'application
volatile sig_atomic_t quit_flag = 0;

// Gestion des signaux, en particulier pour quitter (fermer la fenetre ou ctrl+c)
void signal_handler(int signum) {
	if (signum == SIGINT) quit_flag = 1; 
}

unsigned char was_pressed_this_frame(const bool* old, const bool* new, SDL_Scancode code) {
	return new[code] && !old[code];
}

unsigned char was_released_this_frame(const bool* old, const bool* new, SDL_Scancode code) {
	return !new[code] && old[code];
}

void handle_events(game_state* state, sdl* s, SDL_Event* event) {
	while (SDL_PollEvent(event)) {
		if (event->type == SDL_EVENT_QUIT) quit_flag = 1;  // Quittage de l'application demandée

		if (event->type == SDL_EVENT_DISPLAY_CURRENT_MODE_CHANGED) {
			const SDL_DisplayMode* display = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(s->window));
			if (display)
				state->frame_rate = display->refresh_rate;
			else
				state->frame_rate = 60.f;
		}
	}
}

/// Algorithm de Bresenham pour mettre tous les bits entre 2 points à 1
/// x1, y1 = coordonnées du premier point
/// x2, y2 = coordonnées du second point
/// f = fonction qui actualise un bit du board (attends board_set_bit ou board_unset_bit)
void bitmap_line(board* b, size_t x1, size_t x2, size_t y1, size_t y2, void (*f)(board*, size_t, size_t)) {
	// La souris n'a pas bougé
	if (x1 == x2 && y1 == y2) {
		f(b, x1, y1);
		return;
	}
	long dx = x2 > x1 ? x2 - x1 : x1 - x2; // abs(x2 - x1)
	long dy = y2 > y1 ? y2 - y1 : y1 - y2; // abs(x2 - x1)
	long sx = x1 < x2 ? 1 : -1;
	long sy = y1 < y2 ? 1 : -1;
	long err = dx - dy;

	unsigned long count = 0;
	while (x1 != x2 || y1 != y2) {
		count++;
		if (count > 100) break;
		f(b, x1, y1);
		long e2 = 2 * err;
		if (e2 > -dy) { err -= dy; x1 += sx; }
		if (e2 < dx) { err += dx; y1 += sy; }
	}
}

void handle_inputs(game_state* state, const double* dt, const sdl* s) {
	int nb_inputs = 0;
	const bool* key_states = SDL_GetKeyboardState(&nb_inputs);
	// La on gère les inputs
	// Espace
	if (was_pressed_this_frame(state->previous_key_states, key_states, SDL_SCANCODE_SPACE))
		state->playing = !state->playing;
	// Flèche de droite (simule une étape)
	if (was_pressed_this_frame(state->previous_key_states, key_states, SDL_SCANCODE_RIGHT))
		board_step(state->board, state->board_next_step);

	// U
	if (was_pressed_this_frame(state->previous_key_states, key_states, SDL_SCANCODE_U))
		state->unlimited_simulation = !state->unlimited_simulation;

	// Flèches haut et bas
	if (was_pressed_this_frame(state->previous_key_states, key_states, SDL_SCANCODE_UP)) {
		// Simule plus d'étapes: temps entre 2 étapes plus bas
		state->simulation_speed /= 2.f;
		if (state->simulation_speed <= 1.f / state->frame_rate)
			state->simulation_speed = 1.f / state->frame_rate;
	} else if (was_pressed_this_frame(state->previous_key_states, key_states, SDL_SCANCODE_DOWN)) {
		// Simule plus lentement
		state->simulation_speed *= 2.f;
		if (state->simulation_speed >= 1.f)
			state->simulation_speed = 1.f;
	}

	// La souris
	float mouse_x, mouse_y;
	SDL_GetMouseState(&mouse_x, &mouse_y);
	Uint32 mouseState = SDL_GetMouseState(NULL, NULL);

	bool leftButtonPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_LEFT);
	bool rightButtonPressed = mouseState & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT);

	if (!state->playing && (leftButtonPressed || rightButtonPressed)) {
		size_t x1 = state->last_mouse_position->x / s->block_size;
		size_t x2 = mouse_x / s->block_size;
		size_t y1 = state->last_mouse_position->y / s->block_size;
		size_t y2 = mouse_y / s->block_size;
	
		// On dessine / efface que si les inputs de la souris sont positifs
		if (state->last_mouse_position->x >= 0 && state->last_mouse_position->y >= 0 && mouse_x >= 0 && mouse_y >= 0) {
			if (leftButtonPressed)
				bitmap_line(state->board, x1, x2, y1, y2, board_set_bit);
			else
				bitmap_line(state->board, x1, x2, y1, y2, board_unset_bit);
		}
	}

	// Copie les nouveaux inputs
	for (int i = 0; i < nb_inputs; i++)
		state->previous_key_states[i] = key_states[i];
	// Actualise la dernière position de la souris
	state->last_mouse_position->x = mouse_x;
	state->last_mouse_position->y = mouse_y;
}

void update(game_state* state, const double* dt) {
	state->timer += *dt;
	if (!state->playing) return;
	if (state->unlimited_simulation || state->timer >= state->simulation_speed) {
		state->timer = 0;
		board_step(state->board, state->board_next_step);
	}
}

void main_loop(game_state* state, sdl* s) {
	double dt = 0; // Temps de calcul de la dernière loop
	struct timeval start, end;
	gettimeofday(&start, NULL);
	gettimeofday(&end, NULL);

	SDL_Event event;


	// Boucle principale
	while (!quit_flag) {
		// Temps mis pour la dernière frame
		gettimeofday(&end, NULL);
		dt = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
		gettimeofday(&start, NULL);

		// Evenements (ici juste quitter)
		handle_events(state, s, &event);

		// Gestion des inputs
		handle_inputs(state, &dt, s);

		// Timer pour savoir le temps de calcul de la simulation, et de l'affichage
		struct timeval t1, t2;
		gettimeofday(&t1, NULL);
		// Fonction principale
		update(state, &dt);

		gettimeofday(&t2, NULL);
		double time_taken_sim = (t2.tv_sec - t1.tv_sec) + (t2.tv_usec - t1.tv_usec) / 1000000.0;
		// Temps de simulation
		printf("Time to simulate: %.5fs", time_taken_sim);

		// Affichage
		if (!state->unlimited_simulation || !state->playing) {
			// Reset l'écran
			SDL_SetRenderDrawColor(s->renderer, 0, 0, 0, 255);
			SDL_RenderClear(s->renderer);
			// Dessiner les objets
			sdl_board_print(s, state->board, s->block_size, s->padding);
			SDL_RenderPresent(s->renderer);
		}
		// Temps d'affichage
		gettimeofday(&t1, NULL);
		double time_taken_print = (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) / 1000000.0;
		printf(", render: %.5fs (%.2f fps)\r", time_taken_print, 1 /(time_taken_sim + time_taken_print));
	}
	printf("\n");
}

int main(int argc, char** argv) {
	args args = parse_args(argc, argv);

	if (!is_valid(&args.bools)) return 1;
	if (is_help(&args.bools)) return help();

	omp_set_num_threads(args.threads);

	// Initialisation des variables du jeu
	game_state state;
	state.unlimited_simulation = false;
	state.playing = false;
	state.timer = 0.f;
	int key_states_nb = 0;
	SDL_GetKeyboardState(&key_states_nb);
	state.previous_key_states = calloc(key_states_nb, sizeof(bool));
	state.width = args.game_width; // Largeur du plateau
	state.height = args.game_height; // Hauteur du plateau

	// Variables de la fenêtre
	int win_width = 1920;
	int win_height = 1200;

	sdl* s = sdl_start(win_width, win_height, "Game of life");
	if (!s) return 1;
	s->block_size = args.block_size;
	s->padding = args.padding;

	// Récupération du frame rate de l'écran sur lequel est la fenêtre
	const SDL_DisplayMode* display = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(s->window));
	if (display)
		state.frame_rate = display->refresh_rate;
	else
		state.frame_rate = 60.f;
	// Temps minimum entre 2 actualisations par défaut
	state.simulation_speed = 1.f / state.frame_rate;

	// Création du plateau
	board* b = board_create(state.width, state.height);
	if (!b) {
		sdl_exit(s);
		return 1;
	}
	state.board = b;
	// Création du plateau de l'étape suivante
	board* b2 = board_create(state.width, state.height);
	if (!b2) {
		sdl_exit(s);
		board_free(b);
		return 1;
	}
	state.board_next_step = b2;

	state.last_mouse_position = vector2_create(0, 0);
	if (!state.last_mouse_position) {
		sdl_exit(s);
		board_free(b);
		return 1;
	}

	// Initialise une grille basique (les bords sont mis à 1)
	if (!is_default_pattern(&args.bools)) {
		for (size_t x = 0; x < state.width; x++) {
			board_set_bit(b, x, 0);
			board_set_bit(b, x, state.height - 1);
		}
		for (size_t y = 0; y < state.height; y++) {
			board_set_bit(b, 0, y);
			board_set_bit(b, state.width - 1, y);
		}
	}

	// On initialise la gestion de signaux
	signal(SIGINT, signal_handler);

	main_loop(&state, s);

	// Sortie douce et gentille
	free(state.previous_key_states);
	vector2_destroy(state.last_mouse_position);
	board_free(b);
	board_free(b2);
	sdl_exit(s);
	return 0;
}


