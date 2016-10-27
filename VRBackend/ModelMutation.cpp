#include "ModelMutation.h"

/*void ModelMutation::AddVertexBlock(unsigned int destination_vertex_index, Vertex* new_vertices, unsigned int num_new_vertices) {
	unsigned int vertex_size_in_bytes = new_vertices[0].GetVertexType().GetVertexSize();
	unsigned int existing_data_size = vertex_data.size();
	vertex_data.resize(existing_data_size + num_new_vertices * vertex_size_in_bytes);
	char* new_data_location = vertex_data.data() + existing_data_size;
	for (unsigned int vertex_number = 0; vertex_number < num_new_vertices; vertex_number++) {
		memcpy(new_data_location + vertex_size_in_bytes * vertex_number, new_vertices[vertex_number].GetData(), vertex_size_in_bytes);
	}

	memcpy_map.push_back(std::make_pair(destination_vertex_index * vertex_size_in_bytes, num_new_vertices * vertex_size_in_bytes));
}*/

void ModelMutation::AddVertexBlock(unsigned int destination_vertex_index, Vertices&& new_vertices) {
	vertex_data.emplace_back(make_pair(destination_vertex_index, new_vertices));
}

/*void ModelMutation::ApplyToData(char* data_location) const {
	const char* current_src_location = vertex_data.data();
	for (const std::pair<unsigned int, unsigned int>& memcpy_command : memcpy_map) {
		memcpy(data_location + memcpy_command.first, current_src_location, memcpy_command.second);
		current_src_location += memcpy_command.second;
	}
}*/

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
