#include "vector2.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

vector2* vector2_create(float x, float y) {
	vector2* v = malloc(sizeof(vector2));
	if (!v) return NULL;
	v->x = x;
	v->y = y;
	return v;
}

void vector2_destroy(vector2* v) {
	if (!v) return;
	free(v);
}

// Additionne 2 vecteurs
vector2* vector2_add(vector2* v1, vector2* v2, char inplace) {
	vector2* result = inplace == 1 ? v1 : inplace == 2 ? v2 : vector2_create(0, 0);
	result->x = v1->x + v2->x;
	result->y = v1->y + v2->y;
	return result;
}

// Soustrait 2 vecteurs
vector2* vector2_subtract(vector2* v1, vector2* v2, char inplace) {
	vector2* result = inplace == 1 ? v1 : inplace == 2 ? v2 : vector2_create(0, 0);
	result->x = v1->x - v2->x;
	result->y = v1->y - v2->y;
	return result;
}

// Multiplie un vecteur par un scalaire
vector2* vector2_scale(vector2* v, float scalar, char inplace) {
	if (inplace) {
		v->x *= scalar;
		v->y *= scalar;
		return v;
	}
	vector2* result = vector2_create(v->x * scalar, v->y * scalar);
	return result;
}

// Multiplie 2 vecteurs
float vector2_dot(vector2* v1, vector2* v2) {
	return v1->x * v2->x + v1->y * v2->y;
}

// Module du vecteur
float vector2_length(vector2* v) {
	return sqrt(v->x * v->x + v->y * v->y);
}

// Normalise le vecteur, ou crée une copie normalisée
vector2* vector2_normalize(vector2* v, char inplace) {
	vector2* result = inplace == 1 ? v : vector2_create(v->x, v->y);
	float length = vector2_length(result);
	if (length > 0)
		return vector2_scale(result, 1.0f / length, inplace);
	return result;
}

// Calcul la distance entre 2 vecteurs
float vector2_distance(vector2* v1, vector2* v2) {
	float distX = v2->x - v1->x;
	float distY = v2->y - v1->x;
	return sqrt(distX * distX + distY * distY);
}

vector2* vector2_normalised_direction(vector2* v1, vector2* v2) {
	vector2* result = vector2_add(v1, v2, 0);
	return vector2_normalize(result, 1);
}


void vector2_direction_draw(vector2* direction, vector2* start_position, SDL_Renderer* renderer, float upscale) {
	if (vector2_length(direction) < 0.001f) return;

	float sx = start_position->x;
	float sy = start_position->y;
	float ex = sx + direction->x * upscale;
	float ey = sy + direction->y * upscale;
	SDL_RenderLine(renderer, sx, sy, ex, ey);
	// Calculate the angle of the line
	float angle = atan2(ey - sy, ex - sx);

	// Length of the arrowhead
	int arrowhead_length = 5;

	// Calculate the points of the arrowhead
	int x1 = ex - arrowhead_length * cos(angle - M_PI / 6); // Left side of the arrowhead
	int y1 = ey - arrowhead_length * sin(angle - M_PI / 6);

	int x2 = ex - arrowhead_length * cos(angle + M_PI / 6); // Right side of the arrowhead
	int y2 = ey - arrowhead_length * sin(angle + M_PI / 6);

	// Draw the two lines forming the arrowhead
	SDL_RenderLine(renderer, ex, ey, x1, y1); // Left line of the arrowhead
	SDL_RenderLine(renderer, ex, ey, x2, y2); // Right line of the arrowhead
}

