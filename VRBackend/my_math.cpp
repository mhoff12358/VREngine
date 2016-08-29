#include "my_math.h"

float lerp(float a, float b, float s) {
	return a*(1-s)+b*s;
}

float magnitude(array<float, 3> in_vert) {
	return pow(pow(in_vert[0], 2) + pow(in_vert[1], 2) + pow(in_vert[2], 2), 0.5);
}

float magnitude(array<float, 4> in_vert) {
	return pow(pow(in_vert[0], 2) + pow(in_vert[1], 2) + pow(in_vert[2], 2) + pow(in_vert[3], 2), 0.5);
}

array<float, 3> cross(array<float, 3> u, array<float, 3> v) {
	return array<float, 3>({u[1]*v[2] - u[2]*v[1], u[2]*v[0] - u[0]*v[2], u[0]*v[1]-u[1]*v[0]});
}

float dot(array<float, 3> u, array<float, 3> v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void dump_vector(array<float, 3> v) {
	std::cout << "Dumped vector:\t" << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
}

array<float, 3> vertex_normalize(array<float, 3> in_vert) {
	float mag = pow(pow(in_vert[0], 2) + pow(in_vert[1], 2) + pow(in_vert[2], 2), 0.5);
	return array<float, 3>({{in_vert[0]/mag, in_vert[1]/mag, in_vert[2]/mag}});
}

float randf(float min, float max) {
	return ((float)rand())/RAND_MAX*(max-min)+min;
}

int eucmod(int a, int modulus) {
	return (a % modulus) % modulus;
}