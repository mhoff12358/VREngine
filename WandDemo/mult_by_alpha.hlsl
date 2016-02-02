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
	result.t1.rgb = color_source_tex.Sample(color_source, tex_coord).rgb * alpha_source_tex.Sample(alpha_source, tex_coord).a;
	result.t1.a = alpha_source_tex.Sample(alpha_source, tex_coord).a;
	return result;
}
