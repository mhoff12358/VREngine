#include "vertex_types.hlsli"
#include "pixel_types.hlsli"
#include "cbuff_solid_color.hlsli"

#include "texture0.hlsli"

Color2 main(VertexTexture vin)
{
	Color2 pout;
	float luminence = model_skin.Sample(skin_sampler, vin.tex_coord).a;
	pout.c1 = float4(color.rgb * luminence, luminence);
	pout.c2 = float4(color.rgb * luminence * color.a, luminence);
	return pout;
}
