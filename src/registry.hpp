#pragma once
#include "components.hpp"
#include "sparse_set.hpp"

class Registry {
private:
	Entity next_entity = 0;
public:
	SparseSet<Position> positions;
	SparseSet<Velocity> velocities;
	SparseSet<Sprite> sprites;
	SparseSet<Animator> animators;
	SparseSet<PlayerControlled> player_tags;
	SparseSet<Tilemap> tilemaps;
	SparseSet<Camera> cameras;

	Entity create_entity() {
		return next_entity++;
	}

	void destroy_entity(Entity e) {
		positions.remove(e);
		velocities.remove(e);
		sprites.remove(e);
		animators.remove(e);
		player_tags.remove(e);
		tilemaps.remove(e);
		cameras.remove(e);
	}
};
