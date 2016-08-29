#include "ModelMutation.h"

void ModelMutation::AddVertexBlock(unsigned int destination_vertex_index, Vertex* new_vertices, unsigned int num_new_vertices) {
	unsigned int vertex_size_in_bytes = new_vertices[0].GetVertexType().GetVertexSize();
	unsigned int existing_data_size = vertex_data.size();
	vertex_data.resize(existing_data_size + num_new_vertices * vertex_size_in_bytes);
	char* new_data_location = vertex_data.data() + existing_data_size;
	for (unsigned int vertex_number = 0; vertex_number < num_new_vertices; vertex_number++) {
		memcpy(new_data_location + vertex_size_in_bytes * vertex_number, new_vertices[vertex_number].GetData(), vertex_size_in_bytes);
	}

	memcpy_map.push_back(std::make_pair(destination_vertex_index * vertex_size_in_bytes, num_new_vertices * vertex_size_in_bytes));
}

void ModelMutation::ApplyToData(char* data_location) const {
	const char* current_src_location = vertex_data.data();
	for (const std::pair<unsigned int, unsigned int>& memcpy_command : memcpy_map) {
		memcpy(data_location + memcpy_command.first, current_src_location, memcpy_command.second);
		current_src_location += memcpy_command.second;
	}
}
