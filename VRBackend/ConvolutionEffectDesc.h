#pragma once

#include "stdafx.h"

#include "PipelineStageDesc.h"

class ResourcePool;

#define CONVOLUTION_SIZE 27

class ConvolutionEffectDesc : public ProcessingEffectDesc
{
public:
	ConvolutionEffectDesc(
		string name,
		tuple<string, TextureSignature> output_texture_desc,
		TextureIdent input_texture_ident,
		ResourcePool* resource_pool,
		bool horizonal_convolution,
		const array<float, CONVOLUTION_SIZE>& convolution_data);
	~ConvolutionEffectDesc();

private:
	static array<float, 4 * (CONVOLUTION_SIZE + 1)>* MorphConvolutionData(const array<float, CONVOLUTION_SIZE>& convolution_data);
	static D3D11_BLEND_DESC GetBlendDesc();
	static string LookupShaderName(bool use_horizontal);
};

