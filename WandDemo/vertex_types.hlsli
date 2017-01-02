struct VertexLocation {
	float4 location : SV_POSITION;
};

struct VertexLocationColor {
	float4 location : SV_POSITION;
	float4 color : COLOR;
};

struct VertexTexture {
	float4 location : SV_POSITION;
	float2 tex_coord : TEXCOORD0;
};

struct VertexAll {
	float4 location : SV_POSITION;
	float3 normal : TEXCOORD0;
	float2 tex_coord : TEXCOORD1;
};
