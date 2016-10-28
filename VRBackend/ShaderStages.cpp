#include "stdafx.h"
#include "ShaderStages.h"

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