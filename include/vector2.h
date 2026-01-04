#ifndef VECTOR2_H
#define VECTOR2_H

#include <SDL3/SDL.h>

typedef struct {
    float x;
    float y;
} vector2;

vector2* vector2_create(float x, float y);
void vector2_destroy(vector2* v);

vector2* vector2_add(vector2* v1, vector2* v2, char inplace);
vector2* vector2_subtract(vector2* v1, vector2* v2, char inplace);
vector2* vector2_scale(vector2* v, float scalar, char inplace);

float vector2_dot(vector2* v1, vector2* v2);
float vector2_length(vector2* v);
vector2* vector2_normalize(vector2* v, char inplace);

float vector2_distance(vector2* v1, vector2* v2);
vector2* vector2_normalised_direction(vector2* v1, vector2* v2);

void vector2_direction_draw(vector2* direction, vector2* start_position, SDL_Renderer* renderer, float upscale);

#endif // ! VECTOR2_H
