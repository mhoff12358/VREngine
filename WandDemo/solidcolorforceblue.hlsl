struct VOut
{
    float4 position : SV_POSITION;
};

struct POut
{
	float4 t1 : SV_Target0;
	float4 t2 : SV_Target1;
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

VOut VShader(float4 position : POSITION)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);

    return output;
}

POut PShader(float4 position : SV_POSITION) : SV_TARGET
{
	POut result;
	result.t1 = float4(0, 0, 1, 1);
	result.t2 = result.t1;
	return result;
}
