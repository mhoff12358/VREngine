#ifndef CBUFF
#define CBUFF

cbuffer color_width : register(b4)
{
	float4 color;
	float width;
};

#endif // CBUFF
