#pragma once

#include "Texture.h"

class TextureView
{
public:
	TextureView(int srs, int ss, bool vs, bool ps, ID3D11ShaderResourceView* srv, ID3D11SamplerState* s);
	TextureView(int srs, int ss, bool vs, bool ps, const Texture& texture);
	TextureView();
	~TextureView();

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	void UnPrepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

	static ID3D11ShaderResourceView* clear_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];

private:
	int shader_resource_slot;
	int sampler_slot;
	bool use_in_ps;
	bool use_in_vs;
	ID3D11ShaderResourceView* shader_resource_view;
	ID3D11SamplerState* sampler;
};

