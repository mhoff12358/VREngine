#include "shared_matrices.hlsli"
#include "vertex_types.hlsli"

VertexAll main( VertexAll vin )
{
	matrix <float, 4, 4> mvp = mul(view_projection, model);
	vin.location = mul(mvp, vin.location);
	vin.normal = mul(model_inv_trans, vin.normal);
	return vin;
}
