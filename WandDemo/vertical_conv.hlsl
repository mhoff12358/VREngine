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

static int kernel_size = 27;

cbuffer kernel : register(b2)
{
	float4 height;
	float kernel[27];
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
		result.t1 = result.t1 + kernel[i].x * model_skin.Sample(skin_sampler, float2(tex_coord.x, tex_coord.y - ((trunc(kernel_size / 2) + i) / height.x)));
	}
	return result;
}
