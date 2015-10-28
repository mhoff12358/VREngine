struct VOut
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD0;
	float light_scale : TEXCOORD1;
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

cbuffer light_details : register(b2)
{
	float3 light_direction;
	float ambient_light;
}

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD0, float2 tex_coord : TEXCOORD1)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.tex_coord = tex_coord;
	output.light_scale = max(dot(mul(model_inv_trans, normal).xyz, light_direction) * (1 - ambient_light), 0.0f) + ambient_light;

    return output;
}


float4 PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0, float light_scale : TEXCOORD1) : SV_TARGET
{
    //return float4(0.0f, tex_coord.y, 0.0f, 1.0f);
	float4 texture_sample = model_skin.Sample(skin_sampler, tex_coord);
	return float4(texture_sample.xyz * light_scale, texture_sample.w);
}
