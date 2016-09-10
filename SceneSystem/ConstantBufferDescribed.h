#pragma once

#include "stdafx.h"
#include "VRBackend/ConstantBuffer.h"
#include "GraphicsObject.h"

class DLLSTUFF ConstantBufferDescribed : public ConstantBuffer
{
public:
	static CB_PIPELINE_STAGES GetStageFromString(const string& stage_name);

	ConstantBufferDescribed(
		CB_PIPELINE_STAGES stages,
		const game_scene::actors::ShaderSettingsFormat& format);
	~ConstantBufferDescribed();

	void* EditBufferData() override;
	void* EditBufferData(bool set_dirty) override;
	const void* ReadBufferData() override;
	unsigned int GetBufferDataSize() override;

private:
	vector<char> data;
};

