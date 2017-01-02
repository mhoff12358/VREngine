#include "vertex_types.hlsli"
#include "pixel_types.hlsli"

#include "texture0.hlsli"

Color1 main(VertexTexture vin)
{
	Color1 pout;
	pout.c1.rg = vin.tex_coord;
	pout.c1.ba = float2(1, 1);
	return pout;
}
