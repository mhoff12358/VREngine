#include "shared_matrices.hlsli"

struct VOut
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct POut
{
	float4 t1 : SV_Target0;
};

Texture2D model_skin : register(t0);
sampler skin_sampler : register(s0);

VOut VShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0)
{
	VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.tex_coord = tex_coord;

	return output;
}


POut PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0) : SV_TARGET
{
	//return float4(0.0f, tex_coord.y, 0.0f, 1.0f);
	POut result;
	result.t1 = model_skin.Sample(skin_sampler, tex_coord);
	return result;
}
