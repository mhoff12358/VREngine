#pragma once

#include "stdafx.h"

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
