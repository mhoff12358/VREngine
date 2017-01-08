#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "cbuff_solid_color.hlsli"
#include "lighting_buffers.hlsli"

Color1 main(VertexAllWorld vin)
{
	Color1 pout;
	pout.c1 = color;
	pout.c1.rgb = apply_lighting(pout.c1.rgb, calculate_lighting(vin.world_location, normalize(vin.normal)));
	return pout;
}
