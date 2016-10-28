#include "stdafx.h"
#include "ModelMutation.h"

void ModelMutation::AddVertexBlock(unsigned int destination_vertex_index, Vertices&& new_vertices) {
	vertex_data.emplace_back(make_pair(destination_vertex_index, new_vertices));
}

void ModelMutation::ApplyToData(char* data_location) const {
	for (const pair<unsigned int, Vertices>& new_vertices : vertex_data) {
		const float* data = new_vertices.second.GetData();
		unsigned int byte_size = new_vertices.second.GetTotalByteSize();
		unsigned int byte_offset = new_vertices.first * new_vertices.second.GetVertexType().GetVertexSize();
		memcpy(
			// The destination location is the index of the first vertex to mutate times the size of each vertex.
			data_location + byte_offset,
			// Copy all the bytes in the buffer.
			data,
			byte_size);
	}
}
