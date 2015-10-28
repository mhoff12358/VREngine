#pragma once

#include "stdafx.h"
#include "ConstantBuffer.h"

typedef vector<tuple<string, unsigned int>> LuaConstantBufferFormat;

class ConstantBufferLua : public ConstantBuffer
{
public:
	static CB_PIPELINE_STAGES GetStageFromString(const string& stage_name);

	ConstantBufferLua(CB_PIPELINE_STAGES stages, LuaConstantBufferFormat format);
	~ConstantBufferLua();

	void* EditBufferData() override;
	void* EditBufferData(bool set_dirty) override;
	const void* ReadBufferData() override;
	unsigned int GetBufferDataSize() override;

private:
	vector<char> data;
};

