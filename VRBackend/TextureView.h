#pragma once

#include "stdafx.h"
#include "ShaderStages.h"
class Texture;

class TextureView
{
public:
	TextureView(unsigned int shader_resource_slot, unsigned int sampler_slot, ShaderStages shader_stages, ID3D11ShaderResourceView* srv, ID3D11SamplerState* s);
	TextureView(unsigned int shader_resource_slot, unsigned int sampler_slot, ShaderStages shader_stages, const Texture& texture);
	TextureView();
	~TextureView();

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	void UnPrepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

	static ID3D11ShaderResourceView* clear_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

private:
	unsigned int shader_resource_slot_;
	unsigned int sampler_slot_;
	ShaderStages shader_stages_;
	ID3D11ShaderResourceView* shader_resource_view;
	ID3D11SamplerState* sampler;
};

