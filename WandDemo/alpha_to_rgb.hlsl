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
	result.t1.rgb = model_skin.Sample(skin_sampler, tex_coord).aaa;
	result.t1.a = 1.0f;
	return result;
}
