#pragma once
#include "registry.hpp"
#include <cmath>

namespace Systems {

void Movement(Registry& reg, float delta_time) {
	for(size_t i = 0; i < reg.velocities.size(); i++) {
		Entity e = reg.velocities.dense_entities[i];
		if(reg.positions.has(e)) {
			reg.positions.get(e).x += reg.velocities.get(e).vx * delta_time;
			reg.positions.get(e).y += reg.velocities.get(e).vy * delta_time;
		}
	}
}

void Render(Registry& reg, SDL_Renderer* renderer) {
	for(size_t i = 0; i < reg.sprites.size(); i++) {
		Entity e = reg.sprites.dense_entities[i];
		if(reg.positions.has(e)) {
			const auto& pos = reg.positions.get(e);
			const auto& sprite = reg.sprites.dense_data[i];
			SDL_FRect dst_rect = { pos.x, pos.y, sprite.width * sprite.scale, sprite.height * sprite.scale };
			SDL_FlipMode flip = sprite.facing_left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
			SDL_FRect src_rect = { 0.0f, 0.0f, sprite.width, sprite.height };

			SDL_RenderTextureRotated(renderer, sprite.texture, &src_rect, &dst_rect, 0.0, nullptr, flip);
		}
	}
}

void RenderTilemap(Registry& reg, SDL_Renderer* renderer) {
	for(size_t i = 0; i < reg.tilemaps.size(); i++) {
		const Entity& e = reg.tilemaps.dense_entities[i];
		if(reg.positions.has(e)) {
			const auto& map = reg.tilemaps.dense_data[i];
			const auto& pos = reg.positions.get(e);
			float scaled_size = map.tile_size * map.scale;

			for(int y = 0; y < map.map_rows; y++) {
				for(int x = 0; x < map.map_columns; x++) {
					int tileid = map.layout[y * map.map_columns + x];
					if(tileid < 0) continue;

					SDL_FRect src_rect = {
						(float)(tileid % map.tileset_columns) * map.tile_size,
						(float)(tileid / map.tileset_columns) * map.tile_size,
						(float) map.tile_size,
						(float) map.tile_size
					};

					SDL_FRect dest_rect = {
						pos.x + (float)(x * scaled_size),
						pos.y + (float)(y * scaled_size),
						scaled_size,
						scaled_size
					};

					SDL_RenderTexture(renderer, map.tileset_texture, &src_rect, &dest_rect);
				}
			}
		}
	}
}
}
