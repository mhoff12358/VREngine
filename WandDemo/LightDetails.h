#pragma once

#include <array>
#include <cmath>

#include <D3D11.h>
#include <DirectXMath.h>

#include "ConstantBuffer.h"

struct LightDetails {
	DirectX::XMFLOAT3 light_direction;
	float ambient_light;

	void SetLightDirection(std::array<float, 3> direction);
};

template <>
class ConstantBufferTyped<LightDetails> : public ConstantBufferTypedTemp<LightDetails>{
public:
	ConstantBufferTyped(CB_PIPELINE_STAGES stages) : ConstantBufferTypedTemp(stages) {}
};
