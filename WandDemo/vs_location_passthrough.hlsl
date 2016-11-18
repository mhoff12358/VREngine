#include "shared_matrices.hlsli"
#include "vertex_types.hlsli"

VertexLocation main( VertexLocation vin )
{
	vin.location = mul(model, vin.location);
	vin.location = mul(view_projection, vin.location);
	return vin;
}