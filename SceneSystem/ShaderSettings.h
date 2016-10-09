#pragma once
#include "stdafx.h"
#include "VRBackend/ConstantBuffer.h"

namespace game_scene {

class ShaderSettingsFormat : public vector<tuple<string, int>> {
public:
	template<typename... Args>
	ShaderSettingsFormat(Args... args) : vector<tuple<string, int>>(args...) {}

	bool ShouldCreateBuffer() { return !empty(); }
};

class ShaderSettingsValue : public vector<vector<float>> {
public:
	template<typename... Args>
	ShaderSettingsValue(Args... args) : vector<vector<float>>(args...) {}

	void operator=(const vector<vector<float>>& other) {
		*this = ShaderSettingsValue(other);
	}

	ShaderSettingsFormat GetFormat() const {
		ShaderSettingsFormat format;
		for (const vector<float>& sub_array : *this) {
			format.push_back(tuple<string, int>("float", sub_array.size()));
		}
		return format;
	}

	vector<vector<float>> GetValue() const { return *this; }

	void SetIntoConstantBuffer(ConstantBuffer* buffer) const {
		float* raw_buffer = static_cast<float*>(buffer->EditBufferData(true));
		for (const vector<float>& next_data_chunk : GetValue()) {
			memcpy(raw_buffer, next_data_chunk.data(), sizeof(float) * next_data_chunk.size());
			raw_buffer = raw_buffer + next_data_chunk.size();
		}
	}
};

}  // game_scene
