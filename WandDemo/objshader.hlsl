struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

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

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.color = float4(1.0, 0.0, 0.0, 1.0);

    return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}
