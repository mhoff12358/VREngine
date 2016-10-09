#pragma once

#include "Texture.h"
#include "Shaders.h"

class TextureView
{
public:
	TextureView(int shader_resource_slot, int sampler_slot, ShaderStages shader_stages, ID3D11ShaderResourceView* srv, ID3D11SamplerState* s);
	TextureView(int shader_resource_slot, int sampler_slot, ShaderStages shader_stages, const Texture& texture);
	TextureView();
	~TextureView();

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	void UnPrepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

	static ID3D11ShaderResourceView* clear_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

private:
	int shader_resource_slot_;
	int sampler_slot_;
	ShaderStages shader_stages_;
	ID3D11ShaderResourceView* shader_resource_view;
	ID3D11SamplerState* sampler;
};

