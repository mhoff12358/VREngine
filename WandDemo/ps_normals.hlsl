#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "lighting_buffers.hlsli"

Color1 main(VertexAll vin)
{
	Color1 pout;
	pout.c1.rgb = (calculate_normal(vin.normal) + float3(1, 1, 1)) * 0.5f;
	pout.c1.a = 1.0f;
	return pout;
}
