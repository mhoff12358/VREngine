#include "TextureView.h"
#include <cstring>

ID3D11ShaderResourceView* TextureView::clear_shader_resource_views[] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

TextureView::TextureView(int srs, int ss, bool vs, bool ps, ID3D11ShaderResourceView* srv, ID3D11SamplerState* s)
	: shader_resource_slot(srs), sampler_slot(ss), use_in_vs(vs), use_in_ps(ps), shader_resource_view(srv), sampler(s)
{
}

TextureView::TextureView(int srs, int ss, bool vs, bool ps, const Texture& texture)
	: shader_resource_slot(srs), sampler_slot(ss), use_in_vs(vs), use_in_ps(ps), shader_resource_view(texture.GetShaderResourceView()), sampler(texture.GetSampler())
{
}

TextureView::TextureView() : shader_resource_view(NULL), sampler(NULL)
{
}

TextureView::~TextureView()
{
}


void TextureView::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (shader_resource_view != NULL) {
		if (use_in_ps) {
			device_context->PSSetShaderResources(shader_resource_slot, 1, &shader_resource_view);
		}
		if (use_in_vs) {
			device_context->VSSetShaderResources(shader_resource_slot, 1, &shader_resource_view);
		}
	}
	if (sampler != NULL) {
		if (use_in_ps) {
			device_context->PSSetSamplers(sampler_slot, 1, &sampler);
		}
		if (use_in_vs) {
			device_context->VSSetSamplers(sampler_slot, 1, &sampler);
		}
	}
}

void TextureView::UnPrepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (shader_resource_view != NULL) {
		if (use_in_ps) {
			device_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, clear_shader_resource_views);
		}
		if (use_in_vs) {
			device_context->VSSetShaderResources(shader_resource_slot, 1, &shader_resource_view);
		}
	}
}


bool TextureView::IsDummy() const {
	return (shader_resource_view == NULL) && (sampler == NULL);
}