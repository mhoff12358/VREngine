struct VOut
{
    float4 position : SV_POSITION;
};

cbuffer shared_matrices : register(b0)
{
	matrix <float, 4, 4> view_projection;
};

cbuffer personal_matrices : register(b1)
{
	matrix <float, 4, 4> model;
	matrix <float, 4, 4> model_inv_trans;
};

cbuffer solid_color : register(b2)
{
	float4 color;
};

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);

    return output;
}

float4 PShader(float4 position : SV_POSITION) : SV_TARGET
{
	//return color;
	return float4(0, 0, 1, 1);
}
