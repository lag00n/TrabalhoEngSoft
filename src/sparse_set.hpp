#pragma once

#include <vector>
#include <cassert>

using Entity = size_t;
const size_t NULL_INDEX = -1;

template <typename T>
class SparseSet {
public:
	std::vector<T> dense_data; // os dados
	std::vector<Entity> dense_entities; // mapeia indice denso -> dados
private:
	std::vector<size_t> sparse; // mapeia ID entidade -> indice denso

public:
	size_t size() const {
		return dense_data.size();
	}

	bool has(Entity entity) const {
		return entity < sparse.size() && sparse[entity] != NULL_INDEX;
	}

	void insert(Entity entity, T component) {
		if(has(entity)) {
			dense_data[sparse[entity]] = component;
			return;
		}

		if(entity >= sparse.size()) {
			sparse.resize(entity + 5, NULL_INDEX);
		}

		size_t new_index = dense_data.size();
		dense_data.push_back(component);
		dense_entities.push_back(entity);

		sparse[entity] = new_index;
	}

	T& get(Entity entity) {
		assert(has(entity) && "[ERRO] Tentando acessar componente inexistente!");
		return dense_data[sparse[entity]];
	}

	void remove(Entity entity) {
		if(!has(entity)) return;
		size_t index_of_removed = sparse[entity];
		size_t index_of_last = dense_data.size() - 1;

		if(index_of_removed != index_of_last) {
			dense_data[index_of_removed] = dense_data[index_of_last];
			Entity last_entity = dense_entities[index_of_last];
			dense_entities[index_of_removed] = last_entity;

			sparse[last_entity] = index_of_removed;
		}

		sparse[entity] = NULL_INDEX;
		dense_data.pop_back();
		dense_entities.pop_back();
	}
	typename std::vector<T>::iterator begin() { return dense_data.begin(); }
	typename std::vector<T>::iterator end() { return dense_data.end(); }
};
