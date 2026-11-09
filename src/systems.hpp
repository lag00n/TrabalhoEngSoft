#pragma once
#include "registry.hpp"
#include <cmath>

namespace Systems {

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
}
