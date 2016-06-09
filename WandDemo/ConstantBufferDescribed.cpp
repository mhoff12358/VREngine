#include "stdafx.h"
#include "ConstantBufferDescribed.h"


CB_PIPELINE_STAGES ConstantBufferDescribed::GetStageFromString(const string& stage_name) {
	if (stage_name == "vertex") {
		return CB_PS_VERTEX_SHADER;
	} else if (stage_name == "pixel") {
		return CB_PS_PIXEL_SHADER;
	}
	assert(false);
}

ConstantBufferDescribed::ConstantBufferDescribed(
	CB_PIPELINE_STAGES stages,
	const game_scene::actors::ShaderSettingsFormat& format)
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


ConstantBufferDescribed::~ConstantBufferDescribed()
{
}

void* ConstantBufferDescribed::EditBufferData() {
	SetDirty();
	return (void*)data.data();
}

void* ConstantBufferDescribed::EditBufferData(bool set_dirty) {
	if (set_dirty) {
		SetDirty();
	}
	return data.data();
}

const void* ConstantBufferDescribed::ReadBufferData() {
	return (void*)data.data();
}

unsigned int ConstantBufferDescribed::GetBufferDataSize() {
	return (((data.size() - 1) / 16) + 1) * 16;
}
