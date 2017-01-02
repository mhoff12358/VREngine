#ifndef LIGHTING_BUFFERS
#define LIGHTING_BUFFERS

#include "shared_matrices.hlsli"

struct DirectionalLight {
	float3 direction;
	float3 color;
};

struct PointLight {
	float3 location;
	float3 color;
};

struct AmbientLight {
	float3 color;
};

static const int num_point_lights = 8;
static const int num_directional_lights = 8;

cbuffer lights : register(b2)
{
	AmbientLight ambient_light;
	PointLight point_lights[num_point_lights];
	DirectionalLight directional_lights[num_directional_lights];
};

static const float specular_exponent = 4.0f;

float3 calculate_single_light(float3 normal, DirectionalLight light, float3 camera_vector) {
	float diffuse = max(0, dot(normal, light.direction));
	float3 halfway_vector = (light.direction + camera_vector) * 0.5;
	float specular = max(0, pow(dot(normal, halfway_vector), specular_exponent));
	return light.color * (diffuse + specular);
}

float3 calculate_lighting(float3 world_location, float3 normal) {
	float3 lighting = ambient_light.color;

	float3 camera_vector = normalize(get_camera_location() - world_location);
	
	[unroll(num_point_lights)]
	for (int i = 0; i < num_point_lights; i++) {
		DirectionalLight point_as_direction;
		point_as_direction.direction = normalize(point_lights[i].location - world_location);
		point_as_direction.color = point_lights[i].color;
		lighting = lighting + calculate_single_light(normal, point_as_direction, camera_vector);
	}

	[unroll(num_directional_lights)]
	for (int j = 0; j < num_directional_lights; j++) {
		lighting = lighting + calculate_single_light(normal, directional_lights[j], camera_vector);
	}

	return min(float3(1, 1, 1), lighting);
}

float3 apply_lighting(float3 color, float3 light) {
	return color * light;
}

#endif // LIGHTING_BUFFERS
