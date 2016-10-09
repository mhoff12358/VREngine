#include "TextureView.h"
#include <cstring>

ID3D11ShaderResourceView* TextureView::clear_shader_resource_views[] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };

TextureView::TextureView(int shader_resource_slot, int sampler_slot, ShaderStages shader_stages, ID3D11ShaderResourceView* srv, ID3D11SamplerState* s)
	: shader_resource_slot_(shader_resource_slot), sampler_slot_(sampler_slot), shader_stages_(shader_stages), shader_resource_view(srv), sampler(s)
{
}

TextureView::TextureView(int shader_resource_slot, int sampler_slot, ShaderStages shader_stages, const Texture& texture)
	: shader_resource_slot_(shader_resource_slot), sampler_slot_(sampler_slot), shader_stages_(shader_stages), shader_resource_view(texture.GetShaderResourceView()), sampler(texture.GetSampler())
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
		if (shader_stages_.IsVertexStage()) {
			device_context->VSSetShaderResources(shader_resource_slot_, 1, &shader_resource_view);
		}
		if (shader_stages_.IsGeometryStage()) {
			device_context->GSSetShaderResources(shader_resource_slot_, 1, &shader_resource_view);
		}
		if (shader_stages_.IsPixelStage()) {
			device_context->PSSetShaderResources(shader_resource_slot_, 1, &shader_resource_view);
		}
	}
	if (sampler != NULL) {
		if (shader_stages_.IsVertexStage()) {
			device_context->VSSetSamplers(sampler_slot_, 1, &sampler);
		}
		if (shader_stages_.IsGeometryStage()) {
			device_context->GSSetSamplers(sampler_slot_, 1, &sampler);
		}
		if (shader_stages_.IsPixelStage()) {
			device_context->PSSetSamplers(sampler_slot_, 1, &sampler);
		}
	}
}

void TextureView::UnPrepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (shader_resource_view != NULL) {
		if (shader_stages_.IsVertexStage()) {
			device_context->VSSetShaderResources(shader_resource_slot_, 1, &shader_resource_view);
		}
		if (shader_stages_.IsGeometryStage()) {
			device_context->GSSetShaderResources(shader_resource_slot_, 1, &shader_resource_view);
		}
		if (shader_stages_.IsPixelStage()) {
			device_context->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, clear_shader_resource_views);
		}
	}
}

bool TextureView::IsDummy() const {
	return (shader_resource_view == NULL) && (sampler == NULL);
}