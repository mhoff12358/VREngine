#include "Shaders.h"
#include <cassert>

ShaderStages ShaderStages::GetFromString(const string& name) {
	if (name == "vertex") {
		return ShaderStages(ShaderStages::VERTEX_STAGE);
	} else if (name == "geometry") {
		return ShaderStages(ShaderStages::GEOMETRY_STAGE);
	} else if (name == "pixel") {
		return ShaderStages(ShaderStages::PIXEL_STAGE);
	} else {
		assert(false);
		return ShaderStages(0);
	}
}

VertexShader::VertexShader()
	: input_layout(nullptr), vertex_shader(nullptr) {

}

VertexShader::VertexShader(ID3D11InputLayout* il, ID3D11VertexShader* vs)
	: input_layout(il), vertex_shader(vs) {

}

void VertexShader::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (!IsDummy()) {
		device_context->VSSetShader(vertex_shader, nullptr, 0);
		device_context->IASetInputLayout(input_layout);
	}
}

bool VertexShader::IsDummy() const {
	return vertex_shader == nullptr;
}

PixelShader::PixelShader()
	: pixel_shader(nullptr) {

}

PixelShader::PixelShader(ID3D11PixelShader* ps)
	: pixel_shader(ps) {

}

void PixelShader::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (!IsDummy()) {
		device_context->PSSetShader(pixel_shader, nullptr, 0);
	}
}

bool PixelShader::IsDummy() const {
	return pixel_shader == nullptr;
}

GeometryShader::GeometryShader()
	: geometry_shader(nullptr), is_dummy_(true) {
}

GeometryShader::GeometryShader(bool is_dummy) : geometry_shader(nullptr), is_dummy_(is_dummy) {

}

GeometryShader::GeometryShader(ID3D11GeometryShader* gs)
	: geometry_shader(gs), is_dummy_(false) {

}

void GeometryShader::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (!IsDummy()) {
		device_context->GSSetShader(geometry_shader, nullptr, 0);
	}
}

bool GeometryShader::IsDummy() const {
	return is_dummy_;
}