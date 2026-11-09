#pragma once
#include "registry.hpp"
#include <cmath>

namespace Systems {

void Input(Registry& reg, const bool* state) {
	for(size_t i = 0; i < reg.player_tags.size(); i++) {
		Entity e = reg.player_tags.dense_entities[i];

		if(reg.velocities.has(e) && reg.sprites.has(e) && reg.animators.has(e)) {
			auto& vel = reg.velocities.get(e);
			auto& sprite = reg.sprites.get(e);
			auto& anim = reg.animators.get(e);

			vel.vx = 0.0f; vel.vy = 0.0f;
			bool is_moving = false;

			if(state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D]) {
				vel.vx = vel.speed;
				sprite.facing_left = false;
				is_moving = true;
			}

			if(state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A]) {
				vel.vx = -vel.speed;
				sprite.facing_left = true;
				is_moving = true;
			}

			if(state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W]) {
				vel.vy = -vel.speed;
				is_moving = true;
			}

			if(state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S]) {
				vel.vy = vel.speed;
				is_moving = true;
			}

			int target_row = is_moving ? 1 : 0;
			int target_frames = is_moving ? 8 : 6;

			if(anim.anim_row != target_row) {
				anim.anim_row = target_row;
				anim.frames_in_row = target_frames;
				anim.current_frame = 0;
				anim.anim_timer = 0.0f;
			}
		}
	}
}

void Movement(Registry& reg, float delta_time) {
	for(size_t i = 0; i < reg.velocities.size(); i++) {
		Entity e = reg.velocities.dense_entities[i];
		if(reg.positions.has(e)) {
			reg.positions.get(e).x += reg.velocities.get(e).vx * delta_time;
			reg.positions.get(e).y += reg.velocities.get(e).vy * delta_time;
		}
	}
}

void Animation(Registry& reg, float delta_time) {
	for(size_t i = 0; i < reg.animators.size(); i++) {
		auto &anim = reg.animators.dense_data[i];
		anim.anim_timer += delta_time;

		if(anim.anim_timer >= 0.1f) {
			anim.current_frame++;
			if(anim.current_frame >= anim.frames_in_row) anim.current_frame = 0;
			anim.anim_timer -= 0.1f;
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

			if(reg.animators.has(e)) {
				const auto& anim = reg.animators.get(e);
				SDL_FRect src_rect = {
					anim.current_frame * sprite.width,
					anim.anim_row * sprite.height,
					sprite.width, sprite.height
				};

				SDL_RenderTextureRotated(renderer, sprite.texture, &src_rect, &dst_rect, 0.0, nullptr, flip);
			}	else {
				SDL_RenderTextureRotated(renderer, sprite.texture, nullptr, &dst_rect, 0.0, nullptr, flip);
			}
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
