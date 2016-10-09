#pragma once

#include "RenderResource.h"
#include "Model.h"
#include "ShaderSettings.h"
#include "Shaders.h"
#include "ConstantBuffer.h"
#include "ModelSlice.h"
#include "TextureView.h"

#include <vector>

typedef unsigned char EntityStatusFlags;
extern const EntityStatusFlags ES_NORMAL;
extern const EntityStatusFlags ES_SETTINGS;
extern const EntityStatusFlags ES_DISABLED;
extern const EntityStatusFlags ES_DELETED;

class Entity
{
public:
	static const TextureView dummy_texture_view;

	Entity(EntityStatusFlags stat, Shaders shaders, ShaderSettings ss, Model m, ConstantBuffer* os, TextureView tex = dummy_texture_view);
	Entity(TextureView tex);
	Entity();
	~Entity();

	void Draw(ID3D11Device* device, ID3D11DeviceContext* device_context);

	EntityStatusFlags status;
	ConstantBuffer* object_settings;
	ShaderSettings& MutableShaderSettings();

	void SetPixelShader(PixelShader new_pixel_shader);
	void SetVertexShader(VertexShader new_vertex_shader);
	void SetGeometryShader(GeometryShader new_geometry_shader);

	void ApplyModelMutation(ID3D11DeviceContext* device_context, const ModelMutation& model_mutation);

private:
	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context);

	Shaders shaders_;
	ShaderSettings shader_settings;
	Model model;
	TextureView texture;
};
