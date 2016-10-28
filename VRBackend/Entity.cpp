#include "stdafx.h"
#include "Entity.h"

#include "ConstantBuffer.h"

const TextureView Entity::dummy_texture_view(0, 0, ShaderStages(), NULL, NULL);

const EntityStatusFlags ES_NORMAL = 0x0;
const EntityStatusFlags ES_SETTINGS = 0x1;
const EntityStatusFlags ES_DISABLED = 0x2;
const EntityStatusFlags ES_DELETED = 0x4;

Entity::Entity(EntityStatusFlags stat, Shaders shaders, ShaderSettings ss, Model m, ConstantBuffer* os, TextureView tex)
	: status(stat), shaders_(shaders), object_settings(os), shader_settings(ss), model(m), texture(tex)
{

}

Entity::Entity(TextureView tex) : Entity(ES_SETTINGS, Shaders(), ShaderSettings(), Model(), nullptr, tex) {
}

Entity::Entity()
	: status(ES_DISABLED), object_settings(NULL), shader_settings(), model(), texture()
{

}


Entity::~Entity()
{
}

void Entity::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) {
	shaders_.pixel_shader_.Prepare(device, device_context);
	shaders_.vertex_shader_.Prepare(device, device_context);
	shaders_.geometry_shader_.Prepare(device, device_context);
	shader_settings.Prepare(device, device_context);
	model.Prepare(device, device_context);
	texture.Prepare(device, device_context);

	if (object_settings != NULL) {
		object_settings->PushBuffer(device_context);
		object_settings->Prepare(device, device_context, PER_MODEL_CONSTANT_BUFFER_REGISTER);
	}
}

void Entity::Draw(ID3D11Device* device, ID3D11DeviceContext* device_context) {
	// If this entity has any of the flags set that mean it shouldn't be drawn, don't draw it
	if ((status & (ES_DISABLED | ES_DELETED)) != 0) {
		return;
	}

	// Always prepare this Entity's settings
	Prepare(device, device_context);

	// Only attempt to draw the entity if its not a settings entity
	if ((status & ES_SETTINGS) == 0) {
		device_context->Draw(model.slice.length, model.slice.start);
	}
	else {

	}
}

ShaderSettings& Entity::MutableShaderSettings() {
	return shader_settings;
}

void Entity::SetPixelShader(PixelShader new_pixel_shader) {
	shaders_.pixel_shader_ = new_pixel_shader;
}

void Entity::SetVertexShader(VertexShader new_vertex_shader) {
	shaders_.vertex_shader_ = new_vertex_shader;
}

void Entity::SetGeometryShader(GeometryShader new_geometry_shader) {
	shaders_.geometry_shader_ = new_geometry_shader;
}

void Entity::ApplyModelMutation(ID3D11DeviceContext* device_context, const ModelMutation& model_mutation) {
	model.ApplyModelMutation(device_context, model_mutation);
}