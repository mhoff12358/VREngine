struct GSInput
{
	float4 pos : SV_POSITION;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
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

cbuffer sprite_details : register(b4)
{
	float2 size;
};

[maxvertexcount(6)]
void GShader(
	point GSInput input[1], 
	inout TriangleStream< GSOutput > output
)
{
	matrix <float, 4, 4> model_view = mul(view, model);
	//matrix <float, 4, 4> model_view = view;
	float4 right = float4(model_view[0][0], model_view[0][1], model_view[0][2], 0) * size.x;
	float4 up = float4(model_view[1][0], model_view[1][1], model_view[1][2], 0) * size.y;
	//float4 right = float4(1, 0, 0, 0);
	//float4 up = float4(0, 1, 0, 0);
	//input[0].pos = float4(0, 0, 0.5, 1);
	GSOutput element1;
	element1.pos = mul(view_projection, input[0].pos + right + up);
	element1.tex_coord = float2(1, 0);
	output.Append(element1);
	GSOutput element2;
	element2.pos = mul(view_projection, input[0].pos - right - up);
	element2.tex_coord = float2(0, 1);
	output.Append(element2);
	GSOutput element3;
	element3.pos = mul(view_projection, input[0].pos - right + up);
	element3.tex_coord = float2(0, 0);
	output.Append(element3);
	GSOutput element11;
	element11.pos = mul(view_projection, input[0].pos + right + up);
	element11.tex_coord = float2(1, 0);
	output.Append(element11);
	GSOutput element12;
	element12.pos = mul(view_projection, input[0].pos - right - up);
	element12.tex_coord = float2(0, 1);
	output.Append(element12);
	GSOutput element13;
	element13.pos = mul(view_projection, input[0].pos + right - up);
	element13.tex_coord = float2(1, 1);
	output.Append(element13);
}