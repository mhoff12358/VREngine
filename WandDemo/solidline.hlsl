struct VOut
{
	float4 pos : SV_POSITION;
};

struct GOut
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
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

cbuffer line_details : register(b2)
{
	float4 color;
	float width;
};

VOut VShader(float4 position : SV_POSITION)
{
	VOut output;

	output.pos = mul(model, position);

	return output;
}

[maxvertexcount(6)]
void GShader(
	line VOut input[2],
	inout TriangleStream< GOut > output
)
{
	float3 line_vec = input[1].pos.xyz - input[0].pos.xyz;
	matrix <float, 4, 4> model_view = mul(view, model);
	float3 right = float3(model_view[0][0], model_view[0][1], model_view[0][2]);
	float3 up = float3(model_view[1][0], model_view[1][1], model_view[1][2]);
	float4 perp_vec = width * float4(normalize(cross(cross(right, up), line_vec)), 0);
	/*
	GOut element1;
	element1.pos = mul(view_projection, input[0].pos + perp_vec);
	element1.color = color;
	output.Append(element1);
	GOut element2;
	element2.pos = mul(view_projection, input[1].pos - perp_vec);
	element2.color = color;
	output.Append(element2);
	GOut element3;
	element3.pos = mul(view_projection, input[0].pos - perp_vec);
	element3.color = color;
	output.Append(element3);
	GOut element11;
	element11.pos = mul(view_projection, input[0].pos + perp_vec);
	element11.color = color;
	output.Append(element11);
	GOut element12;
	element12.pos = mul(view_projection, input[1].pos - perp_vec);
	element12.color = color;
	output.Append(element12);
	GOut element13;
	element13.pos = mul(view_projection, input[1].pos + perp_vec);
	element13.color = color;
	output.Append(element13);
	*/
	/*
	GOut element;
	element.color = float4(1, 1, 1, 1);
	element.pos = mul(view_projection, float4(0, 0, 0, 1));
	output.Append(element);
	element.pos = mul(view_projection, float4(1, 0, 0, 1));
	output.Append(element);
	element.pos = mul(view_projection, float4(0, 1, 0, 1));
	output.Append(element);*/
	GOut element;
	element.color = float4(1, 1, 1, 1);
	element.pos = float4(0, 0, -1, 1);
	output.Append(element);
	element.pos = float4(1, 0, -1, 1);
	output.Append(element);
	element.pos = float4(0, 1, -1, 1);
	output.Append(element);
}

POut PShader(GOut output) : SV_TARGET
{
	POut result;
	result.t1 = output.color;
	result.t1 = float4(1, 1, 1, 1);
	return result;
}
