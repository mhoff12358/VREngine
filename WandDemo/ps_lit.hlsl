#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "lighting_buffers.hlsli"

Color1 main(VertexAllWorld vin)
{
	Color1 pout;
	pout.c1.rgb = calculate_lighting(vin.world_location, normalize(vin.normal));
	pout.c1.a = 1.0f;
	return pout;
}
