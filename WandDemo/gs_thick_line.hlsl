#include "vertex_types.hlsli"
#include "shared_matrices.hlsli"
#include "cbuff_color_width.hlsli"

[maxvertexcount(12)]
void main(
	lineadj VertexLocation input[4],
	inout TriangleStream< VertexLocation > output
)
{
	float3 line_vec = input[2].location.xyz - input[1].location.xyz;
	matrix <float, 4, 4> model_view = mul(view, model);
	matrix <float, 4, 4> mvp = mul(view_projection, model);

	float3 right = float3(model_view[0][0], model_view[0][1], model_view[0][2]);
	float3 up = float3(model_view[1][0], model_view[1][1], model_view[1][2]);
	float4 perp_vec = width * float4(normalize(cross(cross(right, up), line_vec)), 0);
	float3 line_vec_adj = input[3].location.xyz - input[2].location.xyz;
	float4 perp_vec_adj = width * float4(normalize(cross(cross(right, up), line_vec_adj)), 0);

	VertexLocation element;
	element.location = mul(mvp, input[1].location + perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location - perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[1].location - perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[1].location + perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location - perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location + perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location - perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location + perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location + perp_vec_adj);
	output.Append(element);
	element.location = mul(mvp, input[2].location - perp_vec);
	output.Append(element);
	element.location = mul(mvp, input[2].location - perp_vec_adj);
	output.Append(element);
	element.location = mul(mvp, input[2].location + perp_vec_adj);
	output.Append(element);
}
