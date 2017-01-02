struct VOut
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
	float light_scale : TEXCOORD1;
};

struct POut
{
	float4 t1 : SV_Target0;
};

Texture2D model_skin : register(t0);
sampler skin_sampler : register(s0);

cbuffer shared_matrices : register(b0)
{
	matrix <float, 4, 4> view;
	matrix <float, 4, 4> view_projection;
};

cbuffer personal_matrices : register(b1)
{
	matrix <float, 4, 4> model;
	matrix <float, 4, 4> model_inv_trans;
};

cbuffer light_details : register(b4)
{
	float3 light_location;
	float ambient_light;
}

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD0, float2 tex_coord : TEXCOORD1)
{
    VOut output;

	output.position = mul(model, position);
	float3 light_direction = light_location - output.position.xyz;
	output.position = mul(view_projection, output.position);
	output.tex_coord = tex_coord;
	output.light_scale = max(dot(normalize(mul(model_inv_trans, normal).xyz), normalize(light_direction)) * (1 - ambient_light), 0.0f) + ambient_light;

    return output;
}


POut PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0, float light_scale : TEXCOORD1) : SV_TARGET
{
    POut result;
	float4 texture_sample = model_skin.Sample(skin_sampler, tex_coord);
	result.t1 = float4(texture_sample.xyz, texture_sample.w);
	return result;
}
