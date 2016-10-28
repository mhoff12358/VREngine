#pragma once

#include "stdafx.h"

enum class ShaderStage : unsigned char {
	VERTEX = 0,
	GEOMETRY = 1,
	PIXEL = 2,
	NUM_STAGES
};

class VertexShader
{
public:
	VertexShader();
	VertexShader(ID3D11InputLayout* il, ID3D11VertexShader* vs);

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

private:
	ID3D11InputLayout* input_layout;
	ID3D11VertexShader* vertex_shader;
};

class PixelShader
{
public:
	PixelShader();
	PixelShader(ID3D11PixelShader* ps);

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

private:
	ID3D11PixelShader* pixel_shader;
};

class GeometryShader
{
public:
	GeometryShader();
	GeometryShader(bool is_dummy);
	GeometryShader(ID3D11GeometryShader* gs);

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

private:
	bool is_dummy_;
	ID3D11GeometryShader* geometry_shader;
};

class Shaders {
public:
	Shaders() {}
	Shaders(VertexShader vertex_shader, PixelShader pixel_shader) :
		vertex_shader_(vertex_shader), pixel_shader_(pixel_shader) {}
	Shaders(VertexShader vertex_shader, PixelShader pixel_shader, GeometryShader geometry_shader) :
		vertex_shader_(vertex_shader), pixel_shader_(pixel_shader), geometry_shader_(geometry_shader) {}

	VertexShader vertex_shader_;
	PixelShader pixel_shader_;
	GeometryShader geometry_shader_;
};