struct VOut
{
	float4 position : SV_POSITION;
};

struct GIn
{
	float4 pos : SV_POSITION;
};

struct GOut
{
	float4 pos : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct POut
{
	float4 t1 : SV_Target0;
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

cbuffer sprite_details : register(b2)
{
	float2 size;
};

VOut VShader(float4 position : POSITION)
{
	VOut output;

	output.position = mul(model, position);

	return output;
}

[maxvertexcount(3 * 2 * 8)]
void GShader(
	point GIn input[1],
	inout TriangleStream< GOut > output
)
{
	matrix <float, 4, 4> model_view = mul(view, model);
	float4 right = float4(model_view[0][0], model_view[0][1], model_view[0][2], 0) * size.x;
	float4 up = float4(model_view[1][0], model_view[1][1], model_view[1][2], 0) * size.y;
	GOut element;
	for (uint i = 0; i < 8; i++) {
		float i_ang = 3.1416 / 8.0 * i;
		float i1_ang = 3.1416 / 8.0 * (i + 1);
		float cos_i = cos(i_ang);
		float cos_i1 = cos(i1_ang);
		float sin_i = sin(i_ang);
		float sin_i1 = sin(i1_ang);
		float4 right_cos_i = cos_i * right;
		float4 up_sin_i = sin_i * up;
		float4 right_cos_i1 = cos_i1 * right;
		float4 up_sin_i1 = sin_i1 * up;
		element.pos = mul(view_projection, input[0].pos - right);
		element.tex_coord = float2(0, 0.5);
		output.Append(element);
		element.pos = mul(view_projection, input[0].pos + right_cos_i - up_sin_i);
		element.tex_coord = float2(0.5 + 0.5 * cos_i, 0.5 + 0.5 * sin_i);
		output.Append(element);
		element.pos = mul(view_projection, input[0].pos + right_cos_i1 - up_sin_i1);
		element.tex_coord = float2(0.5 + 0.5 * cos_i1, 0.5 + 0.5 * sin_i1);
		output.Append(element);
		element.pos = mul(view_projection, input[0].pos - right);
		element.tex_coord = float2(0, 0.5);
		output.Append(element);
		element.pos = mul(view_projection, input[0].pos + right_cos_i + up_sin_i);
		element.tex_coord = float2(0.5 + 0.5 * cos_i, 0.5 - 0.5 * sin_i);
		output.Append(element);
		element.pos = mul(view_projection, input[0].pos + right_cos_i1 + up_sin_i1);
		element.tex_coord = float2(0.5 + 0.5 * cos_i1, 0.5 - 0.5 * sin_i1);
		output.Append(element);
	}
}

POut PShader(float4 position : SV_POSITION, float2 tex_coord : TEXCOORD0) : SV_TARGET
{
	POut result;
result.t1 = model_skin.Sample(skin_sampler, tex_coord);
return result;
}
