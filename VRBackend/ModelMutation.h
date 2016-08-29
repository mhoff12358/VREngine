#pragma once

#include "Vertices.h"

#include <vector>

class ModelMutation
{
public:
	// destination vertex index is a reference into the vertex buffer in terms of number of vertices.
	// It will be multiplied by the size of the new_vertices vertex type's size in order to get the total offset.
	void AddVertexBlock(unsigned int destination_vertex_index, Vertex* new_vertices, unsigned int num_new_vertices);

	void ApplyToData(char* data_location) const;

private:
	std::vector<char> vertex_data;
	// Each pair has two unsigned ints. The first is an offset into the vertex buffer in bytes,
	// the second is a size of the vertex_data that should be copied to that destination.
	std::vector<std::pair<unsigned int, unsigned int>> memcpy_map;
};

