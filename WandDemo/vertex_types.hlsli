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