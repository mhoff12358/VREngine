#include "stdafx.h"
#include "Lights.h"

const unsigned int LightSystem::lighting_register = 2;

const int AmbientLight::cb_size = sizeof(float) * 4;
const int PointLight::cb_size = sizeof(float) * 8;
const int DirectionalLight::cb_size = sizeof(float) * 8;

void AmbientLight::WriteToBuffer(char* buffer) const {
	memcpy(buffer, color_.rgba_.data(), sizeof(color_.rgba_));
}

void PointLight::WriteToBuffer(char* buffer) const {
	memcpy(buffer, location_.location_.data(), sizeof(location_.location_));
	memcpy(buffer + sizeof(float) * 4, color_.rgba_.data(), sizeof(color_.rgba_));
}

void DirectionalLight::WriteToBuffer(char* buffer) const {
	memcpy(buffer, direction_.location_.data(), sizeof(direction_.location_));
	memcpy(buffer + sizeof(float) * 4, color_.rgba_.data(), sizeof(color_.rgba_));
}

LightSystem::LightSystem() : dirty_(true) {

}

LightSystem::LightSystem(
	AmbientLight base_ambient_light,
	array<PointLight, num_point_lights> point_lights,
	array<DirectionalLight, num_directional_lights> directional_lights) :
	base_ambient_light_(base_ambient_light),
	point_lights_(std::move(point_lights)),
	directional_lights_(std::move(directional_lights)),
	dirty_(true)
{

}

unsigned int LightSystem::GetConstantBufferSize() const {
	return AmbientLight::cb_size + point_lights_.size() * PointLight::cb_size + directional_lights_.size() * DirectionalLight::cb_size;
}

void LightSystem::SetConstantBuffer(ConstantBuffer* constant_buffer) {
	constant_buffer_ = constant_buffer;
}

void LightSystem::WriteToBuffer(char* buffer) {
	total_ambient_light_.WriteToBuffer(buffer);
	buffer += AmbientLight::cb_size;

	for (const PointLight& point_light : point_lights_) {
		point_light.WriteToBuffer(buffer);
		buffer += PointLight::cb_size;
	}

	for (const DirectionalLight& directional_light : directional_lights_) {
		directional_light.WriteToBuffer(buffer);
		buffer += DirectionalLight::cb_size;
	}
}

void LightSystem::WriteToConstantBuffer(ConstantBuffer* constant_buffer) {
	WriteToBuffer(static_cast<char*>(constant_buffer->EditBufferData()));
}

void LightSystem::Push(ID3D11DeviceContext* device_context) {
	if (dirty_) {
		ComputeTotalAmbientLight();
		WriteToConstantBuffer(constant_buffer_);
		constant_buffer_->PushBuffer(device_context);
		dirty_ = false;
	}
}

void LightSystem::ComputeTotalAmbientLight() {
	total_ambient_light_ = base_ambient_light_;
	for (const PointLight& point_light : point_lights_) {
		total_ambient_light_.color_ = point_light.color_.ScaleRGB(0.1f).ApplyOnto(total_ambient_light_.color_);
	}
}

void LightSystem::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) {
	constant_buffer_->Prepare(device, device_context, lighting_register);
}

void LightSystem::Update(const LightSystem& other) {
	*this = other;
}

AmbientLight& LightSystem::MutableAmbientLight() {
	dirty_ = true;
	return base_ambient_light_;
}

array<PointLight, LightSystem::num_point_lights>& LightSystem::MutablePointLights() {
	dirty_ = true;
	return point_lights_;
}

array<DirectionalLight, LightSystem::num_directional_lights>& LightSystem::MutableDirectionalLights() {
	dirty_ = true;
	return directional_lights_;
}

unsigned int LightSystem::ClaimPointLight() {
	unsigned int i;
	for (i = 0; i < num_point_lights; i++) {
		if (!point_lights_claimed_[i]) {
			break;
		}
	}
	return i;
}

void LightSystem::ReleasePointLight(unsigned int light_number) {
	point_lights_claimed_[light_number] = false;
}

unsigned int LightSystem::ClaimDirectionalLight() {
	unsigned int i;
	for (i = 0; i < num_directional_lights; i++) {
		if (!directional_lights_claimed_[i]) {
			break;
		}
	}
	return i;
}

void LightSystem::ReleaseDirectionalLight(unsigned int light_number) {
	directional_lights_claimed_[light_number] = false;
}

