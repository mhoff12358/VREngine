struct VOut
{
    float4 position : SV_POSITION;
    float2 tex_coord : TEXCOORD;
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

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD0, float2 tex_coord : TEXCOORD1)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.tex_coord = tex_coord;

    return output;
}


float4 PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD) : SV_TARGET
{
    //return float4(tex_coord.x, tex_coord.y, 0.0f, 1.0f);
	return model_skin.Sample(skin_sampler, tex_coord);
}
