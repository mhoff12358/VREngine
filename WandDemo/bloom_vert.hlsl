struct VOut
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct POut
{
	float4 t1 : SV_Target0;
};

Texture2D source_tex : register(t0);
sampler source : register(s0);

static int kernel_size = 51;

cbuffer kernel : register(b2)
{
	float4 height;
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
		float4 adjacent_sample = source_tex.Sample(source, float2(tex_coord.x, tex_coord.y + ((-trunc(kernel_size / 2) + i) / height.x)));
		result.t1 = max(result.t1, float4(kernel[i].x * adjacent_sample.a * adjacent_sample.xyz, adjacent_sample.a));
		//result.t1 = max(result.t1, adjacent_sample);
	}
	return result;
}
