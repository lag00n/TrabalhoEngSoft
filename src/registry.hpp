#pragma once
#include "components.hpp"
#include "sparse_set.hpp"

class Registry {
private:
	Entity next_entity = 0;
public:
	SparseSet<Position> positions;

	Entity create_entity() {
		return next_entity++;
	}

	void destroy_entity(Entity e) {
		positions.remove(e);
	}
};
