#include "shared_matrices.hlsli"
#include "vertex_types.hlsli"

VertexAllWorld main( VertexAll vin )
{
	VertexAllWorld vout;
	matrix <float, 4, 4> mvp = mul(view_projection, model);
	vout.world_location = mul(model, vin.location);
	vout.location = mul(view_projection, vout.world_location);
	vout.normal = mul(model_inv_trans, float4(vin.normal, 0)).xyz;
	vout.tex_coord = vin.tex_coord;
	return vout;
}
