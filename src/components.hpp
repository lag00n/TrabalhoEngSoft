#pragma once
#include <SDL3/SDL.h>
#include <vector>

// Uma entidade eh apenas um id no vetor
using Entity = size_t;

struct Position {
	float x, y;
};

struct Sprite {
	bool facing_left;
	float width, height, scale;
	SDL_Texture* texture;
};

struct Tilemap {
	SDL_Texture* tileset_texture;
	std::vector<int> layout;
	int tile_size;
	int tileset_columns;
	int map_columns;
	int map_rows;
	float scale;
};
