#pragma once

/*
#include <array>
#include <cmath>

#include <D3D11.h>
#include <DirectXMath.h>

#include "ConstantBuffer.h"

struct LightDetails {
	DirectX::XMFLOAT3 light_direction;
	float ambient_light;

	// Sets the direction that the light shines in. This is a vector from the light source to the object.
	// This is scaled by -1 in order to be in the proper form for the shader.
	void SetLightDirection(std::array<float, 3> direction);
	// Sets the direction to the light source. This is a vector from the object the the light source.
	void SetLightSourceDirection(std::array<float, 3> direction);
};

template <>
class ConstantBufferTyped<LightDetails> : public ConstantBufferTypedTemp<LightDetails>{
public:
	ConstantBufferTyped(CB_PIPELINE_STAGES stages) : ConstantBufferTypedTemp(stages) {}
};
*/