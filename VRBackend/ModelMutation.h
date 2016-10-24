#pragma once

#include "stl.h"
#include "Vertices.h"

#include <vector>

class ModelMutation
{
public:
	ModelMutation() {}

	void AddVertexBlock(unsigned int destination_vertex_index, Vertices&& new_vertices);

	void ApplyToData(char* data_location) const;

private:
	// The first value in the pair is the index of the first vertex that should be mutated.
	vector<pair<unsigned int, Vertices>> vertex_data;
};

