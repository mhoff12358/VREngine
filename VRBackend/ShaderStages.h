#pragma once

#include "stdafx.h"

struct ShaderStage {
public:
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

	ShaderStage() : ShaderStage(0) {}

	static ShaderStage Vertex() { return ShaderStage(VERTEX_STAGE); }
	static ShaderStage Geometry() { return ShaderStage(GEOMETRY_STAGE); }
	static ShaderStage Pixel() { return ShaderStage(PIXEL_STAGE); }

	static ShaderStage GetFromString(const string& name);
	static ShaderStage GetFromStageNumber(unsigned char stage_number) {
		switch (stage_number) {
		case VERTEX_NUMBER:
			return ShaderStage(VERTEX_STAGE);
		case PIXEL_NUMBER:
			return ShaderStage(PIXEL_STAGE);
		case GEOMETRY_NUMBER:
			return ShaderStage(GEOMETRY_STAGE);
		default:
			return ShaderStage(0);
		}
	}

	bool IsVertexStage() const { return (stages_ & VERTEX_STAGE) != 0; }
	bool IsGeometryStage() const { return (stages_ & GEOMETRY_STAGE) != 0; }
	bool IsPixelStage() const { return (stages_ & PIXEL_STAGE) != 0; }

	unsigned char ToStageNumber() const {
		switch (stages_) {
		case VERTEX_STAGE:
			return VERTEX_NUMBER;
		case GEOMETRY_STAGE:
			return GEOMETRY_NUMBER;
		case PIXEL_STAGE:
			return PIXEL_NUMBER;
		default:
			return NUMBER_STAGES;
		}
	}

	unsigned char GetStages() const { return stages_;  } 

protected:
	explicit ShaderStage(unsigned char id) : stages_(id) {
		assert(id == VERTEX_STAGE || id == GEOMETRY_STAGE || id == PIXEL_STAGE || id == 0);
	}

	unsigned char stages_;
};

struct ShaderStages : public ShaderStage {
public:	
	ShaderStages(const ShaderStage& stage) : ShaderStage(stage) {}
	ShaderStages(const vector<ShaderStage>& stages) : ShaderStage(0) {
		for (const ShaderStage& stage : stages) {
			stages_ |= stage.GetStages();
		}
	}
	ShaderStages(unsigned char id) : ShaderStage() { stages_ = id; }
	ShaderStages() : ShaderStage() {}
	
	static ShaderStages All() { return ShaderStages(VERTEX_STAGE | GEOMETRY_STAGE | PIXEL_STAGE); }

	ShaderStages and(ShaderStages other) { return ShaderStages(stages_ | other.stages_); }
};
