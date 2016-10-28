#include "stdafx.h"
#include "ConvolutionEffectDesc.h"

#include "ResourcePool.h"
#include "PipelineStageDesc.h"
#include "ProcessingEffect.h"

ConvolutionEffectDesc::ConvolutionEffectDesc(
	string name,
	tuple<string, TextureSignature> output_texture_desc,
	TextureIdent input_texture_ident,
	ResourcePool* resource_pool,
	bool horizonal_convolution,
	const array<float, CONVOLUTION_SIZE>& convolution_data)
	: ProcessingEffectDesc(
		name,
		{ output_texture_desc },
		{ input_texture_ident },
		GetBlendDesc(),
		resource_pool->LoadPixelShader(LookupShaderName(horizonal_convolution)),
		resource_pool->LoadVertexShader(LookupShaderName(horizonal_convolution),
			ProcessingEffect::squares_vertex_type),
			(char*)MorphConvolutionData(convolution_data),
		sizeof(array<float, 4 * (CONVOLUTION_SIZE + 1)>))
{
}

ConvolutionEffectDesc::~ConvolutionEffectDesc()
{
}

D3D11_BLEND_DESC ConvolutionEffectDesc::GetBlendDesc() {
	D3D11_BLEND_DESC overwrite_blend_desc;
	overwrite_blend_desc.AlphaToCoverageEnable = false;
	overwrite_blend_desc.IndependentBlendEnable = false;
	overwrite_blend_desc.RenderTarget[0].BlendEnable = true;
	overwrite_blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	overwrite_blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	overwrite_blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	overwrite_blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	overwrite_blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	overwrite_blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	overwrite_blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	return overwrite_blend_desc;
}

const string& ConvolutionEffectDesc::LookupShaderName(bool use_horizontal) {
	if (use_horizontal) {
		return "horizontal_conv.hlsl";
	}
	return "vertical_conv.hlsl";
}