#pragma once
#include <SDL3/SDL.h>
#include <vector>

// Uma entidade eh apenas um id no vetor
using Entity = size_t;

struct Position {
	float x, y;
};

struct Velocity {
	float vx, vy;
	float speed;
};

struct Sprite {
	bool facing_left;
	float width, height, scale;
	SDL_Texture* texture;
};

struct Animator {
	int current_frame;
	int anim_row;
	int frames_in_row;
	float anim_timer;
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

struct PlayerControlled {};
