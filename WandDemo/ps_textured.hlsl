#include "vertex_types.hlsli"
#include "pixel_types.hlsli"

#include "texture0.hlsli"

Color1 main(VertexAll vin)
{
	Color1 pout;
	pout.c1 = model_skin.Sample(skin_sampler, vin.tex_coord);
	return pout;
}
