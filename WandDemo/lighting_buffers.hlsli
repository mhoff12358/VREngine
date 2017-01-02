struct DirectionalLight {
	float3 direction;
	float4 color;
};

struct PointLight {
	float3 location;
	float4 color;
};

struct AmbientLight {
	float4 color;
};

static const int num_point_lights = 8;
static const int num_directional_lights = 8;

cbuffer lights : register(b2)
{
	AmbientLight ambient_light;
	PointLight point_lights[num_point_lights];
	DirectionalLight directional_lights[num_directional_lights];
};

float3 calculate_normal(float3 normal_vector) {
	float3 normal = normalize(normal_vector);
	return normal;
}