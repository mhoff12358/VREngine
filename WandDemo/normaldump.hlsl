struct VOut
{
	float4 position : SV_POSITION;
	float3 normal : TEXCOORD;
};

Texture2D model_skin : register(t0);
sampler skin_sampler : register(s0);

cbuffer shared_matrices : register(b0)
{
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
	//output.tex_coord = tex_coord;
	//output.light_scale = dot(mul(model_inv_trans, normal).xyz, light_direction) * (1 - ambient_light) + ambient_light;
	output.normal = mul(model_inv_trans, normal).xyz;

	return output;
}


float4 PShader(float4 position : SV_POSITION, float3 normal : TEXCOORD) : SV_TARGET
{
	return float4(normal, 1.0f);
}
