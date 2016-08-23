struct VOut
{
    float4 position : SV_POSITION;
	float color_position : TEXCOORD0;
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
	float4 color_1;
	float4 color_2;
	float color_boundary;
};

VOut VShader(float4 position : POSITION, float2 tex_coord : TEXCOORD1)
{
    VOut output;

	output.position = mul(model, position);
	output.position = mul(view_projection, output.position);
	output.color_position = tex_coord.x;

    return output;
}

POut PShader(VOut input) : SV_TARGET
{
	int after_color_barrier = ceil(input.color_position - color_boundary);
	POut return_val;
	return_val.t1 = color_1 * (1 - after_color_barrier) + color_2 * after_color_barrier;
	//return_val.t1 = float4(color_boundary, color_boundary, color_boundary, 1.0f);
	//return_val.t1 = float4(input.color_position, input.color_position, input.color_position, 1.0f);
	return return_val;
}
