#ifndef PIXEL_TYPES
#define PIXEL_TYPES

struct Color1
{
	float4 c1 : SV_Target0;
};

struct Color2
{
	float4 c1 : SV_Target0;
	float4 c2 : SV_Target1;
};

#endif // PIXEL_TYPES