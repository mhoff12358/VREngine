#include "vertex_types.hlsli"
#include "shared_matrices.hlsli"
#include "cbuff_color_width.hlsli"

[maxvertexcount(12)]
void main(
	lineadj VertexLocation input[4],
	inout TriangleStream< VertexLocation > output
)
{
	float4 i0 = mul(model, input[0].location);
	float4 i1 = mul(model, input[1].location);
	float4 i2 = mul(model, input[2].location);

	float3 line_vec = i2.xyz - i1.xyz;

	float4 camera_vec = mul(view_inverse, float4(0, 0, 0, 1));
	float3 perp_vec_0 = normalize(cross(camera_vec.xyz - i0.xyz, i1.xyz - i0.xyz));
	float3 perp_vec_1 = normalize(cross(camera_vec.xyz - i1.xyz, line_vec));
	float3 perp_vec_2 = normalize(cross(camera_vec.xyz - i2.xyz, line_vec));

	VertexLocation element;

	element.location = mul(view_projection, i1);
	output.Append(element);
	element.location = mul(view_projection, i1 + float4(width * perp_vec_0, 0));
	output.Append(element);
	element.location = mul(view_projection, i1 + float4(width * perp_vec_1, 0));
	output.Append(element);
	element.location = mul(view_projection, i1);
	output.Append(element);
	element.location = mul(view_projection, i1 - float4(width * perp_vec_0, 0));
	output.Append(element);
	element.location = mul(view_projection, i1 - float4(width * perp_vec_1, 0));
	output.Append(element);

	element.location = mul(view_projection, i1 + float4(width * perp_vec_1, 0));
	output.Append(element);
	element.location = mul(view_projection, i1 - float4(width * perp_vec_1, 0));
	output.Append(element);
	element.location = mul(view_projection, i2 - float4(width * perp_vec_2, 0));
	output.Append(element);
	element.location = mul(view_projection, i1 + float4(width * perp_vec_1, 0));
	output.Append(element);
	element.location = mul(view_projection, i2 - float4(width * perp_vec_2, 0));
	output.Append(element);
	element.location = mul(view_projection, i2 + float4(width * perp_vec_2, 0));
	output.Append(element);
}
