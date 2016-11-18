struct VOut
{
    float4 position : SV_POSITION;
};

struct POut
{
	float4 t1 : SV_Target0;
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

cbuffer solid_color : register(b2)
{
	float4 color;
};

VOut VShader(float4 position : SV_POSITION)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);

    return output;
}

POut PShader(float4 position : SV_POSITION) : SV_TARGET
{
	POut return_val;
	return_val.t1 = color.rgba;
	return return_val;
	//return float4(1, 1, 1, 1);
}
