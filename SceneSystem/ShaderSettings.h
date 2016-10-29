#pragma once
#include "stdafx.h"

class ConstantBuffer;

namespace game_scene {

class ShaderSettingsFormat : public vector<tuple<string, int>> {
public:
	ShaderSettingsFormat();
	template<typename... Args>
	ShaderSettingsFormat(Args... args);

	bool ShouldCreateBuffer();
};

class ShaderSettingsValue : public vector<vector<float>> {
public:
	ShaderSettingsValue();
	template<typename... Args>
	ShaderSettingsValue(Args... args);

	void operator=(const vector<vector<float>>& other);

	ShaderSettingsFormat GetFormat() const;

	vector<vector<float>> GetValue() const;

	void SetIntoConstantBuffer(ConstantBuffer* buffer) const;
};

}  // game_scene
