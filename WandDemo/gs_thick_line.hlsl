#include "vertex_types.hlsli"
#include "shared_matrices.hlsli"
#include "cbuff_color_width.hlsli"

[maxvertexcount(6)]
void main(
	line VertexLocation input[2],
	inout TriangleStream< VertexLocation > output
)
{
	float3 line_vec = input[1].location.xyz - input[0].location.xyz;
	matrix <float, 4, 4> model_view = mul(view, model);
	float3 right = float3(model_view[0][0], model_view[0][1], model_view[0][2]);
	float3 up = float3(model_view[1][0], model_view[1][1], model_view[1][2]);
	float4 perp_vec = width * float4(normalize(cross(cross(right, up), line_vec)), 0);
	/*
	VertexLocation element1;
	element1.pos = mul(view_projection, input[0].pos + perp_vec);
	element1.color = color;
	output.Append(element1);
	VertexLocation element2;
	element2.pos = mul(view_projection, input[1].pos - perp_vec);
	element2.color = color;
	output.Append(element2);
	VertexLocation element3;
	element3.pos = mul(view_projection, input[0].pos - perp_vec);
	element3.color = color;
	output.Append(element3);
	VertexLocation element11;
	element11.pos = mul(view_projection, input[0].pos + perp_vec);
	element11.color = color;
	output.Append(element11);
	VertexLocation element12;
	element12.pos = mul(view_projection, input[1].pos - perp_vec);
	element12.color = color;
	output.Append(element12);
	VertexLocation element13;
	element13.pos = mul(view_projection, input[1].pos + perp_vec);
	element13.color = color;
	output.Append(element13);
	*/
	/*
	VertexLocation element;
	element.color = float4(1, 1, 1, 1);
	element.pos = mul(view_projection, float4(0, 0, 0, 1));
	output.Append(element);
	element.pos = mul(view_projection, float4(1, 0, 0, 1));
	output.Append(element);
	element.pos = mul(view_projection, float4(0, 1, 0, 1));
	output.Append(element);*/
	VertexLocation element;
	element.location = float4(0, 0, -1, 1);
	output.Append(element);
	element.location = float4(1, 0, -1, 1);
	output.Append(element);
	element.location = float4(0, 1, -1, 1);
	output.Append(element);
}
