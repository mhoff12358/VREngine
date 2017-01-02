struct VOut
{
	float4 position : SV_POSITION;
	float light_scale : TEXCOORD0;
	float2 refraction_offset : TEXCOORD1;
};

Texture2D existing_back_buffer : register(t0);
sampler back_buffer_sampler : register(s0);

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

cbuffer solid_color : register(b4)
{
	float3 light_direction;
	float refractivity;
	float3 reflection_color;
	float specularity_power;
	float minimum_reflection;
};

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD)
{
    VOut output;

	output.position = mul(model, position);
	float3 view_vector = -normalize(mul(view, output.position).xyz);
	float3 normal_vector = normalize(mul(model_inv_trans, normal).xyz);
	output.position = mul(view_projection, output.position);
	output.light_scale = max(pow(dot(normalize(view_vector + light_direction), normal_vector), specularity_power), 0);
	output.refraction_offset = normalize(normal_vector.xy) * refractivity;
	output.refraction_offset.y *= -1;

    return output;
}

float4 PShader(float4 position : SV_POSITION, float light_scale : TEXCOORD0, float2 refraction_offset : TEXCOORD1) : SV_TARGET
{
	float4 back_buffer_value = existing_back_buffer.Sample(back_buffer_sampler, float2(position.x / 800.0 + refraction_offset.x, position.y / 600 + refraction_offset.y));
	float alpha_value = 0.15 + max((1-0.15) * light_scale, 0);
	return float4(back_buffer_value.xyz, 1.0) * (1 - alpha_value) + float4(float3(1, 1, 1), 1) * alpha_value;
}
