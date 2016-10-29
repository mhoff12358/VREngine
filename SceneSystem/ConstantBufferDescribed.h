#pragma once

#include "stdafx.h"

#include "VRBackend/ConstantBuffer.h"
#include "ShaderSettings.h"

class ConstantBufferDescribed : public ConstantBuffer
{
public:
	ConstantBufferDescribed(
		ShaderStages stages,
		const game_scene::ShaderSettingsFormat& format);
	ConstantBufferDescribed(
		ShaderStages stages,
		const game_scene::ShaderSettingsValue& initial_value,
		ID3D11Device* device_interface);
	~ConstantBufferDescribed();

	void* EditBufferData() override;
	void* EditBufferData(bool set_dirty) override;
	const void* ReadBufferData() override;
	unsigned int GetBufferDataSize() override;

private:
	vector<char> data;
};

