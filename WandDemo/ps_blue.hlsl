#include "vertex_types.hlsli"
#include "pixel_types.hlsli"

Color1 main(VertexLocation vin)
{
	Color1 pout;
	pout.c1 = float4(0.0f, 0.0f, 1.0f, 1.0f);
	return pout;
}