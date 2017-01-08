#pragma once

#include "stdafx.h"

#include "Color.h"
#include "ConstantBuffer.h"

typedef string LightingSystemIdent;

struct AmbientLight {
	Color color_;

	static const int cb_size;
	void WriteToBuffer(char* buffer) const;
};

struct PointLight {
	Location location_;
	Color color_;

	static const int cb_size;
	void WriteToBuffer(char* buffer) const;
};

struct DirectionalLight {
	Location direction_;
	Color color_;

	static const int cb_size;
	void WriteToBuffer(char* buffer) const;
};

class LightSystem {
public:
	static constexpr size_t num_point_lights = 8;
	static constexpr size_t num_directional_lights = 8;

	LightSystem();
	LightSystem(
		AmbientLight base_ambient_light,
		array<PointLight, num_point_lights> point_lights,
		array<DirectionalLight, num_directional_lights> directional_lights);

	unsigned int GetConstantBufferSize() const;
	void SetConstantBuffer(ConstantBuffer* constant_buffer);
	void WriteToBuffer(char* buffer);
	void WriteToConstantBuffer(ConstantBuffer* constant_buffer);
	void Push(ID3D11DeviceContext* device_context);
	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context);

	AmbientLight& MutableAmbientLight();
	array<PointLight, num_point_lights>& MutablePointLights();
	array<DirectionalLight, num_directional_lights>& MutableDirectionalLights();

	unsigned int ClaimPointLight();
	void ReleasePointLight(unsigned int light_number);
	unsigned int ClaimDirectionalLight();
	void ReleaseDirectionalLight(unsigned int light_number);

	void Update(const LightSystem& other);

private:
	void ComputeTotalAmbientLight();

	AmbientLight base_ambient_light_;
	AmbientLight total_ambient_light_;
	array<PointLight, num_point_lights> point_lights_;
	array<bool, num_point_lights> point_lights_claimed_;
	array<DirectionalLight, num_directional_lights> directional_lights_;
	array<bool, num_directional_lights> directional_lights_claimed_;

	ConstantBuffer* constant_buffer_ = nullptr;

	bool dirty_;

	static const unsigned int lighting_register;
};
