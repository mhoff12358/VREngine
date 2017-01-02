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
		AmbientLight ambient_light,
		array<PointLight, num_point_lights> point_lights,
		array<DirectionalLight, num_directional_lights> directional_lights);

	unsigned int GetConstantBufferSize() const;
	void SetConstantBuffer(ConstantBuffer* constant_buffer);
	void WriteToBuffer(char* buffer);
	void WriteToConstantBuffer(ConstantBuffer* constant_buffer);
	void Push(ID3D11DeviceContext* device_context);
	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context);

	void Update(const LightSystem& other);

private:
	AmbientLight ambient_light_;
	array<PointLight, num_point_lights> point_lights_;
	array<DirectionalLight, num_directional_lights> directional_lights_;

	ConstantBuffer* constant_buffer_ = nullptr;

	bool dirty_;

	static const unsigned int lighting_register;
};
