struct VOut
{
    float4 position : SV_POSITION;
	float fluid_height : TEXCOORD;
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

cbuffer solid_color : register(b4)
{
	float4 color;
	float fluid_level;
};

VOut VShader(float4 position : POSITION, float4 normal : TEXCOORD)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.fluid_height = position.y;

    return output;
}

POut PShader(float4 position : SV_POSITION, float fluid_height : TEXCOORD) : SV_TARGET
{
	POut return_val;
	return_val.t1 = color.rgba * ((sign(fluid_level - fluid_height) + 1) / 2);
	return_val.t2 = return_val.t1;
	return return_val;
	//return float4(1, 1, 1, 1);
}
