#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "lighting_buffers.hlsli"

#include "texture0.hlsli"

Color1 main(VertexAllWorld vin)
{
	Color1 pout;
	pout.c1 = model_skin.Sample(skin_sampler, vin.tex_coord);
	//pout.c1.rgb = apply_lighting(pout.c1.rgb, calculate_lighting(vin.world_location, vin.normal));
	pout.c1.rgb = calculate_lighting(vin.world_location, vin.normal);
	return pout;
}
