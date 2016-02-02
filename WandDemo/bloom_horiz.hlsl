struct VOut
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct POut
{
	float4 t1 : SV_Target0;
};

Texture2D color_source_tex : register(t0);
sampler color_source : register(s0);
Texture2D alpha_source_tex : register(t1);
sampler alpha_source : register(s1);

static int kernel_size = 51;

cbuffer kernel : register(b2)
{
	float4 width;
	float kernel[51];
};

VOut VShader(float4 position : POSITION, float2 tex_coord : TEXCOORD0)
{
	VOut output;

	output.position = float4(position.xy, 0.5, 1);
	output.tex_coord = tex_coord;

	return output;
}


POut PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0) : SV_TARGET
{
	POut result;
	result.t1 = float4(0, 0, 0, 0);
	[unroll(kernel_size)]
	for (int i = 0; i < kernel_size; i++) {
		float4 adjacent_sample = color_source_tex.Sample(color_source, float2(tex_coord.x + ((-trunc(kernel_size / 2) + i) / width.x), tex_coord.y));
		float4 adjacent_alpha_sample = alpha_source_tex.Sample(alpha_source, float2(tex_coord.x + ((-trunc(kernel_size / 2) + i) / width.x), tex_coord.y));
		result.t1 = max(result.t1, kernel[i].x * adjacent_alpha_sample.a * adjacent_sample);
	}
	//result.t1 = result.t1 + model_skin.Sample(skin_sampler, tex_coord);
	return result;
}
