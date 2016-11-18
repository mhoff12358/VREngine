#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "cbuff_solid_color.hlsli"

Color1 main(VertexLocation vin)
{
	Color1 pout;
	pout.c1 = color;
	return pout;
}
