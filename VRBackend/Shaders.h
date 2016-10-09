#pragma once

#include <D3D11.h>
#include "stl.h"

enum class ShaderStage : unsigned char {
	VERTEX = 0,
	GEOMETRY = 1,
	PIXEL = 2,
	NUM_STAGES
};

struct ShaderStages {
	enum StageId : unsigned char {
		VERTEX_STAGE = 0x1,
		GEOMETRY_STAGE = 0x2,
		PIXEL_STAGE = 0x4,
	};

	enum StageNumber : unsigned char {
		VERTEX_NUMBER = 0,
		GEOMETRY_NUMBER = 1,
		PIXEL_NUMBER = 2,
		NUMBER_STAGES = 3
	};

	explicit ShaderStages(unsigned char id) : stages_(id) {}
	ShaderStages() : ShaderStages(0) {}

	static ShaderStages Vertex() { return ShaderStages(VERTEX_STAGE); }
	static ShaderStages Geometry() { return ShaderStages(GEOMETRY_STAGE); }
	static ShaderStages Pixel() { return ShaderStages(PIXEL_STAGE); }
	static ShaderStages All() { return ShaderStages(VERTEX_STAGE | GEOMETRY_STAGE | PIXEL_STAGE); }

	ShaderStages and(ShaderStages other) { return ShaderStages(stages_ | other.stages_); }

	static ShaderStages GetFromString(const string& name);
	static ShaderStages GetFromStageNumber(unsigned char stage_number) {
		switch (stage_number) {
		case VERTEX_NUMBER:
			return ShaderStages(VERTEX_STAGE);
		case PIXEL_NUMBER:
			return ShaderStages(PIXEL_STAGE);
		case GEOMETRY_NUMBER:
			return ShaderStages(GEOMETRY_STAGE);
		default:
			return ShaderStages(0);
		}
	}

	bool IsVertexStage() const { return (stages_ & VERTEX_STAGE) != 0; }
	bool IsGeometryStage() const { return (stages_ & GEOMETRY_STAGE) != 0; }
	bool IsPixelStage() const { return (stages_ & PIXEL_STAGE) != 0; }

	unsigned char stages_;
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