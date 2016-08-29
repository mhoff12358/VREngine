#pragma once

#include <fstream>
#include <vector>
#include <array>
#include <map>

#include "stl.h"
#include "Model.h"
#include "ModelGenerator.h"
#include "Vertices.h"

// For axis a in the output model, look up value v from axis axis_swap[a], and use v*axis_scale[a]
struct ModelModifier {
	ModelModifier() :
		ModelModifier({0, 1, 2}, {1, 1, 1}, {false, false}) {}
	ModelModifier(
		array<int, 3> axis_swap,
		array<float, 3> axis_scale,
		array<bool, 2> invert_texture_axis) :
		axis_swap_(axis_swap),
		axis_scale_(axis_scale),
		invert_texture_axis_(invert_texture_axis) {}

	array<int, 3> axis_swap_;
	array<float, 3> axis_scale_;
	array<bool, 2> invert_texture_axis_;
	static ModelModifier identity_model_modifier;
};

class ObjLoader
{
public:
	struct OutputFormat {
		OutputFormat() {}
		OutputFormat(ModelModifier model_modifier, VertexType vertex_type, bool load_as_dynamic) :
			model_modifier_(model_modifier),
			vertex_type_(vertex_type),
			load_as_dynamic_(load_as_dynamic) {}

		ModelModifier model_modifier_;
		VertexType vertex_type_;
		bool load_as_dynamic_;
	};

	static VertexType vertex_type_location;
	static VertexType vertex_type_texture;
	static VertexType vertex_type_normal;
	static VertexType vertex_type_all;
	static const OutputFormat default_output_format;

	static ModelGenerator CreateModelFromFile(ID3D11Device* device, ID3D11DeviceContext* device_context, std::string filename, const OutputFormat& output_format = default_output_format);
	static ModelGenerator CreateModelsFromFile(ID3D11Device* device, ID3D11DeviceContext* device_context, std::string filename, const OutputFormat& output_format = default_output_format);

	ObjLoader(std::string filename);

	void AddLinesToModel(ID3D11Device* device, ID3D11DeviceContext* device_context, const std::vector<std::string>& input_lines, const OutputFormat& output_format, ModelGenerator& generator, std::string part_name = "");
	ModelGenerator ParseAsMultipleModels(ID3D11Device* device, ID3D11DeviceContext* device_context, const OutputFormat& output_format);
	ModelGenerator ParseAsSingleModel(ID3D11Device* device, ID3D11DeviceContext* device_context, const OutputFormat& output_format);

	Vertex CreateVertex(std::string vertex_id);
	Vertex CreateVertex(std::string location_str, std::string texture_coords_str, std::string normal_str);
	Vertex CreateVertexFromAll(int location_ref, int texture_coords_ref, int normal_ref);
	Vertex CreateVertexFromTexture(int location_ref, int texture_coords_ref);
	Vertex CreateVertexFromNormal(int location_ref, int normal_ref);
	Vertex CreateVertexFromLocation(int location_ref);

private:
	std::ifstream input_file;

	std::vector<std::array<float, 3>> locations;
	std::vector<std::array<float, 2>> texture_coords;
	std::vector<std::array<float, 3>> normals;
};

