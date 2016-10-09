#include "stdafx.h"
#include "ConstantBufferDescribed.h"

ConstantBufferDescribed::ConstantBufferDescribed(
	ShaderStages stages,
	const game_scene::ShaderSettingsFormat& format)
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

ConstantBufferDescribed::ConstantBufferDescribed(
	ShaderStages stages,
	const game_scene::ShaderSettingsValue& value,
	ID3D11Device* device_interface)
	: ConstantBufferDescribed(stages, value.GetFormat())
{
	CreateBuffer(device_interface);
	value.SetIntoConstantBuffer(this);
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
