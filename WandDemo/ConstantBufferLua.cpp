#include "stdafx.h"
#include "ConstantBufferLua.h"


CB_PIPELINE_STAGES ConstantBufferLua::GetStageFromString(const string& stage_name) {
	if (stage_name == "vertex") {
		return CB_PS_VERTEX_SHADER;
	} else if (stage_name == "pixel") {
		return CB_PS_PIXEL_SHADER;
	}
	assert(false);
}

ConstantBufferLua::ConstantBufferLua(CB_PIPELINE_STAGES stages, LuaConstantBufferFormat format)
	: ConstantBuffer(stages)
{
	size_t total_size = 0;
	for (const tuple<string, unsigned int>& element : format) {
		size_t data_type_size = 0;
		if (std::get<0>(element) == "float") {
			data_type_size = sizeof(float);
		}
		assert(data_type_size != 0);
		total_size += data_type_size * std::get<1>(element);
	}
	data.resize(total_size);
}


ConstantBufferLua::~ConstantBufferLua()
{
}

void* ConstantBufferLua::EditBufferData() {
	SetDirty();
	return (void*)data.data();
}

void* ConstantBufferLua::EditBufferData(bool set_dirty) {
	if (set_dirty) {
		SetDirty();
	}
	return data.data();
}

const void* ConstantBufferLua::ReadBufferData() {
	return (void*)data.data();
}

unsigned int ConstantBufferLua::GetBufferDataSize() {
	return data.size();
}
