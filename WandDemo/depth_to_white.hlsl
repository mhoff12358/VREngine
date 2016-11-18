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
	float a = (log(model_skin.Sample(skin_sampler, tex_coord)*0.9 + 0.1) + 2.3026) / 2.3026;
	result.t1.r = a;
	result.t1.g = a;
	result.t1.b = a;
	result.t1.a = 1.0f;
	return result;
}
