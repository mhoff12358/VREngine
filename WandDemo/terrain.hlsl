struct VOut
{
	float4 position : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct POut
{
	float4 t1 : SV_Target0;
	float4 t2 : SV_Target1;
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

cbuffer terrain_details : register(b2)
{
	float terrain_resolution;
	float3 terrain_height_scale;
}

VOut VShader(float4 position : POSITION)
{
	VOut output;

	output.tex_coord = ((position.xy / terrain_resolution) + 1) / 2;
	float3 texture_sample = float3(0, 0, 0);
	texture_sample.x = model_skin.GatherRed(skin_sampler, output.tex_coord);
	texture_sample.y = model_skin.GatherGreen(skin_sampler, output.tex_coord);
	texture_sample.z = model_skin.GatherBlue(skin_sampler, output.tex_coord);
	float height = dot((float3(0.5, 0.5, 0.5) - texture_sample), terrain_height_scale);
	//output.position = float4(position.xy, , position.w);
	output.position = float4(position.x, position.y, height, position.w);


	output.position = mul(model, output.position);
	output.position = mul(view_projection, output.position);

	output.tex_coord = texture_sample.xy;
	
	return output;
}


POut PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0) : SV_TARGET
{
	POut result;
	float4 texture_sample = model_skin.Sample(skin_sampler, tex_coord);
	result.t1 = texture_sample.xyzw;
	result.t1 = float4(tex_coord, 0, 1);
	result.t2 = result.t1;
	return result;
}
