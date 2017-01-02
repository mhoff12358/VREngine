#pragma once

#include "stdafx.h"

class ConstantBuffer;

class ShaderSettings
{
public:
	ShaderSettings();
	ShaderSettings(ConstantBuffer* buffer);
	~ShaderSettings();

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

	ConstantBuffer* constant_buffer;

private:
	static const int buffer_register = 4;
};
