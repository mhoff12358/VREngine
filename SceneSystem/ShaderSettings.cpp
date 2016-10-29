#pragma once
#include "stdafx.h"
#include "ShaderSettings.h"
#include "VRBackend/ConstantBuffer.h"

namespace game_scene {

ShaderSettingsFormat::ShaderSettingsFormat() : vector<tuple<string, int>>() {}

template<typename... Args>
ShaderSettingsFormat::ShaderSettingsFormat(Args... args) : vector<tuple<string, int>>(args...) {}

bool ShaderSettingsFormat::ShouldCreateBuffer() { return !empty(); }

ShaderSettingsValue::ShaderSettingsValue() : vector<vector<float>>() {}

template<typename... Args>
ShaderSettingsValue::ShaderSettingsValue(Args... args) : vector<vector<float>>(args...) {}

void ShaderSettingsValue::operator=(const vector<vector<float>>& other) {
	*this = ShaderSettingsValue(other);
}

ShaderSettingsFormat ShaderSettingsValue::GetFormat() const {
	ShaderSettingsFormat format;
	for (const vector<float>& sub_array : *this) {
		format.push_back(tuple<string, int>("float", sub_array.size()));
	}
	return format;
}

vector<vector<float>> ShaderSettingsValue::GetValue() const { return *this; }

void ShaderSettingsValue::SetIntoConstantBuffer(ConstantBuffer* buffer) const {
	float* raw_buffer = static_cast<float*>(buffer->EditBufferData(true));
	for (const vector<float>& next_data_chunk : GetValue()) {
		memcpy(raw_buffer, next_data_chunk.data(), sizeof(float) * next_data_chunk.size());
		raw_buffer = raw_buffer + next_data_chunk.size();
	}
}

}  // game_scene
