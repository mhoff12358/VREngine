#include "stdafx.h"
#include "ObjLoader.h"

#include "ModelSlice.h"
#include "EntityHandler.h"

#include <sstream>

ModelModifier ModelModifier::identity_model_modifier = {
	{ 0, 1, 2 },
	{ 1.0f, 1.0f, 1.0f },
	{ false, false }
};

ModelGenerator ObjLoader::CreateModelsFromFile(ID3D11Device* device, std::string filename, const OutputFormat& output_format) {
	ObjLoader loader(filename);
	return loader.ParseAsMultipleModels(device, output_format);
}

const ObjLoader::OutputFormat ObjLoader::default_output_format = {
	ModelModifier::identity_model_modifier,
	VertexType::vertex_type_all,
	false
};

ObjLoader::ObjLoader(std::string filename)
	: input_file(filename) {
}

void ObjLoader::AddLinesToModel(ID3D11Device* device, const std::vector<std::string>& input_lines, const OutputFormat& output_format, ModelGenerator& generator, std::string part_name) {
	ModelSlice new_part;
	new_part.start = generator.GetCurrentNumberOfVertices();
	
	for (const std::string& line : input_lines) {
		std::istringstream line_stream(line);

		std::string identifier;
		line_stream >> identifier;

		if (identifier == "v") {
			std::string x_str, y_str, z_str;
			line_stream >> x_str >> y_str >> z_str;
			std::array<float, 3> raw_locations = { { static_cast<float>(atof(x_str.c_str())), static_cast<float>(atof(y_str.c_str())), static_cast<float>(atof(z_str.c_str())) } };
			std::array<float, 3> result_locations;
			for (int i = 0; i < 3; i++) {
				result_locations[i] = raw_locations[output_format.model_modifier_.axis_swap_[i]] * output_format.model_modifier_.axis_scale_[i];
			}
			locations.push_back(result_locations);
		}
		else if (identifier == "vn") {
			std::string x_str, y_str, z_str;
			line_stream >> x_str >> y_str >> z_str;
			std::array<float, 3> raw_normals = { { static_cast<float>(atof(x_str.c_str())), static_cast<float>(atof(y_str.c_str())), static_cast<float>(atof(z_str.c_str())) } };
			std::array<float, 3> result_normals;
			for (int i = 0; i < 3; i++) {
				result_normals[i] = raw_normals[output_format.model_modifier_.axis_swap_[i]] * output_format.model_modifier_.axis_scale_[i];
			}
			normals.push_back(result_normals);
		}
		else if (identifier == "vt") {
			std::string u_str, v_str;
			line_stream >> u_str >> v_str;
			std::array<float, 2> result_texture_coords = { { static_cast<float>(atof(u_str.c_str())), static_cast<float>(atof(v_str.c_str())) } };
			for (int i = 0; i < 2; i++) {
				if (output_format.model_modifier_.invert_texture_axis_[i]) {
					result_texture_coords[i] = 1.0f - result_texture_coords[i];
				}
			}
			texture_coords.push_back(result_texture_coords);
		}
		else if (identifier == "f") {
			std::vector<std::string> vertex_ids;
			std::string test_vertex_id;
			while (line_stream >> test_vertex_id) {
				vertex_ids.push_back(test_vertex_id);
			}
			for (int i = 1; i < vertex_ids.size() - 1; i++) {
				generator.AddVertex(CreateVertex(vertex_ids[0]));
				generator.AddVertex(CreateVertex(vertex_ids[i + 1]));
				generator.AddVertex(CreateVertex(vertex_ids[i]));
			}
		}
	}

	if (!part_name.empty()) {
		new_part.length = generator.GetCurrentNumberOfVertices() - new_part.start;
		generator.parts_[part_name] = new_part;
	}
}

ModelGenerator ObjLoader::ParseAsMultipleModels(ID3D11Device* device, const OutputFormat& output_format) {
	std::string line;
	std::string existing_sub_object_name = "";
	std::vector<std::string> current_model_lines;

	ModelGenerator generator(output_format.vertex_type_, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	while (std::getline(input_file, line)) {
		if (line.empty()) {
			continue;
		}
		std::istringstream line_stream(line);
		std::string identifier;
		line_stream >> identifier;
		if (identifier == "o") {
			AddLinesToModel(device, current_model_lines, output_format, generator, existing_sub_object_name);
			existing_sub_object_name = "";
			line_stream >> existing_sub_object_name;
			current_model_lines.clear();
		} else {
			current_model_lines.push_back(line);
		}
	}
	AddLinesToModel(device, current_model_lines, output_format, generator, existing_sub_object_name);

	ModelStorageDescription model_storage_description;
	if (output_format.load_as_dynamic_) {
		model_storage_description = { false, true, true };
	}
	else {
		model_storage_description = { true, false, false };
	}
	generator.Finalize(device, optional<EntityHandler&>{}, model_storage_description);
	return generator;
}

Vertex ObjLoader::CreateVertex(std::string vertex_id) {
	std::string location_id_str("");
	std::string tex_coord_id_str("");
	std::string normal_id_str("");

	size_t first_slash = vertex_id.find("/");
	if (first_slash == std::string::npos) {
		location_id_str = vertex_id;
	}
	else {
		location_id_str = vertex_id.substr(0, first_slash);
		size_t second_slash = vertex_id.find("/", first_slash + 1);
		if (second_slash == std::string::npos) {
			tex_coord_id_str = vertex_id.substr(first_slash + 1, vertex_id.length() - first_slash - 1);
		}
		else {
			tex_coord_id_str = vertex_id.substr(first_slash + 1, second_slash - first_slash - 1);
			normal_id_str = vertex_id.substr(second_slash + 1, vertex_id.length() - second_slash - 1);
		}
	}
	return CreateVertex(location_id_str, tex_coord_id_str, normal_id_str);
}

Vertex ObjLoader::CreateVertex(std::string location_str, std::string texture_coords_str, std::string normal_str) {
	if (texture_coords_str == "") {
		if (normal_str == "") {
			return CreateVertexFromLocation(atoi(location_str.c_str())-1);
		}
		return CreateVertexFromNormal(atoi(location_str.c_str())-1, atoi(normal_str.c_str())-1);
	}
	if (normal_str == "") {
		return CreateVertexFromTexture(atoi(location_str.c_str())-1, atoi(texture_coords_str.c_str())-1);
	}
	return CreateVertexFromAll(atoi(location_str.c_str())-1, atoi(texture_coords_str.c_str())-1, atoi(normal_str.c_str())-1);
}

Vertex ObjLoader::CreateVertexFromAll(int location_ref, int texture_coords_ref, int normal_ref) {
	std::vector<float> vector_data;
	vector_data.insert(vector_data.end(), locations[location_ref].begin(), locations[location_ref].end());
	vector_data.insert(vector_data.end(), normals[normal_ref].begin(), normals[normal_ref].end());
	vector_data.insert(vector_data.end(), texture_coords[texture_coords_ref].begin(), texture_coords[texture_coords_ref].end());
	return Vertex(VertexType::vertex_type_all, vector_data);
}

Vertex ObjLoader::CreateVertexFromTexture(int location_ref, int texture_coords_ref) {
	std::vector<float> vector_data;
	vector_data.insert(vector_data.end(), locations[location_ref].begin(), locations[location_ref].end());
	vector_data.insert(vector_data.end(), texture_coords[texture_coords_ref].begin(), texture_coords[texture_coords_ref].end());
	return Vertex(VertexType::vertex_type_texture, vector_data);
}

Vertex ObjLoader::CreateVertexFromNormal(int location_ref, int normal_ref) {
	std::vector<float> vector_data;
	vector_data.insert(vector_data.end(), locations[location_ref].begin(), locations[location_ref].end());
	vector_data.insert(vector_data.end(), normals[normal_ref].begin(), normals[normal_ref].end());
	return Vertex(VertexType::vertex_type_normal, vector_data);
}

Vertex ObjLoader::CreateVertexFromLocation(int location_ref) {
	std::vector<float> vector_data;
	vector_data.insert(vector_data.end(), locations[location_ref].begin(), locations[location_ref].end());
	return Vertex(VertexType::vertex_type_location, vector_data);
}