#include "stdafx.h"
#include "ResourceIdentifier.h"

std::string ResourceIdentifier::object_delimiter = "|";
std::string ResourceIdentifier::unique_identifier = "?";
int ResourceIdentifier::unique_number = 0;

bool ShaderIdentifier::IsPrecompiledFile(const string& file_name) {
	return (file_name.rfind(".cso") == (file_name.size() - 4));
}

ShaderIdentifier ShaderIdentifier::GetUnsetShader(ShaderStage stage) {
	if (stage.IsVertexStage()) {
		return ShaderIdentifier(ResourceIdentifier::GetConstantModelName("unset_vertex_shader"), stage);
	}
	if (stage.IsGeometryStage()) {
		return ShaderIdentifier(ResourceIdentifier::GetConstantModelName("unset_geometry_shader"), stage);
	}
	//if (stage.IsPixelStage()) {
		return ShaderIdentifier(ResourceIdentifier::GetConstantModelName("unset_pixel_shader"), stage);
	//}
}

ShaderIdentifier::ShaderIdentifier() {
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	ShaderStage shader_stage) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(shader_stage) {
	assert(!shader_stage_.IsVertexStage());
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	ShaderStage shader_stage,
	VertexType vertex_type) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(shader_stage),
	vertex_type_(vertex_type) {
	assert(shader_stage.IsVertexStage());
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	VertexType vertex_type) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(ShaderStage::Vertex()),
	vertex_type_(vertex_type) {
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	string function_name,
	ShaderStage shader_stage) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(shader_stage),
	function_name_(function_name) {
	assert(!shader_stage_.IsVertexStage());
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	string function_name,
	ShaderStage shader_stage,
	VertexType vertex_type) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(shader_stage),
	function_name_(function_name),
	vertex_type_(vertex_type) {
	assert(shader_stage_.IsVertexStage());
}

ShaderIdentifier::ShaderIdentifier(
	string file_name,
	string function_name,
	VertexType vertex_type) :
	precompiled_(IsPrecompiledFile(file_name)),
	file_name_(file_name),
	shader_stage_(ShaderStage::Vertex()),
	function_name_(function_name),
	vertex_type_(vertex_type) {
}

const string& ShaderIdentifier::GetFileName() const {
	return file_name_;
}

const string& ShaderIdentifier::GetFunctionName() const {
	return function_name_;
}

const VertexType& ShaderIdentifier::GetVertexType() const {
	return vertex_type_;
}

const ShaderStage& ShaderIdentifier::GetShaderStage() const {
	return shader_stage_;
}

bool ShaderIdentifier::GetPrecompiled() const {
	return precompiled_;
}

bool ShaderIdentifier::IsEmpty() const {
	return file_name_.empty();
}
