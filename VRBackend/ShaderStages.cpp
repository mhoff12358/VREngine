#include "stdafx.h"
#include "ShaderStages.h"

ShaderStage ShaderStage::GetFromString(const string& name) {
	if (name == "vertex") {
		return ShaderStage(ShaderStage::VERTEX_STAGE);
	} else if (name == "geometry") {
		return ShaderStage(ShaderStage::GEOMETRY_STAGE);
	} else if (name == "pixel") {
		return ShaderStage(ShaderStage::PIXEL_STAGE);
	} else {
		assert(false);
		return ShaderStage(0);
	}
}