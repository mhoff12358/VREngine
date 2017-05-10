#include "stdafx.h"
#include "Quaternion.h"

float dot(const array<float, 3>& u, const array<float, 3>& v) {
	return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

array<float, 3> cross(const array<float, 3>& u, const array<float, 3>& v) {
	return array<float, 3>({ u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0] });
}

unsigned int get_nonzero_index(const array<float, 3>& u, const array<float, 3>& v) {
	for (unsigned int i = 0; i < 3; i++) {
		if (!FloatsAlmostEqual(u[i], 0) && !FloatsAlmostEqual(v[i], 0)) {
			return i;
		}
	}
	return 3;
}

bool get_same_direction(const array<float, 3>& u, const array<float, 3>& v) {
	unsigned int i = get_nonzero_index(u, v);
	if ((i < 3) && u[i] / v[i] < 0) {
		return false;
	}
	return true;
}

array<float, 3> vertex_normalize(array<float, 3> in_vert) {
	float mag = pow(pow(in_vert[0], 2) + pow(in_vert[1], 2) + pow(in_vert[2], 2), 0.5f);
	return array<float, 3>({ { in_vert[0] / mag, in_vert[1] / mag, in_vert[2] / mag } });
}

Quaternion::Quaternion() {
	x = 0;
	y = 0;
	z = 0;
	w = 1;
}

Quaternion::Quaternion(std::array<float, 4> val) {
	x = val[0];
	y = val[1];
	z = val[2];
	w = val[3];
}

Quaternion::Quaternion(float* val) {
	x = val[0];
	y = val[1];
	z = val[2];
	w = val[3];
}

Quaternion::Quaternion(float xval, float yval, float zval, float wval) :
	x(xval), y(yval), z(zval), w(wval) {

}

std::array<float, 4> Quaternion::GetArray() const {
	return { { x, y, z, w } };
}

float Quaternion::dot(const Quaternion& other) const {
	return x*other.x + y*other.y + z*other.z + w*other.w;
}

std::array<float, 3> Quaternion::ApplyToVector(std::array<float, 3> input_vec) const {
	Quaternion inv = *this;
	inv.x *= -1;
	inv.y *= -1;
	inv.z *= -1;
	Quaternion result = (*this) * Quaternion(input_vec[0], input_vec[1], input_vec[2], 0) * inv;
	return std::array<float, 3>({ result.x, result.y, result.z });
}

Quaternion Quaternion::operator*(const Quaternion& other) const {
	return Quaternion(
		w*other.x + x*other.w + y*other.z - z*other.y,
		w*other.y - x*other.z + y*other.w + z*other.x,
		w*other.z + x*other.y - y*other.x + z*other.w,
		w*other.w - x*other.x - y*other.y - z*other.z);
}

Quaternion Quaternion::operator*(const float other) const {
	return Quaternion(x*other, y*other, z*other, w*other);
}

Quaternion Quaternion::ToPower(const float& other) const {
	// To avoid a divide by zero scenario, if theta is 0 (meaning w is 1) then
	// the end result is just the same quaternion
	if (w == 1.0f) {
		return Quaternion(0, 0, 0, 1);
	}
	float theta = acos(w) * 2.0f;
	float imaginary_scale = sin(theta*other / 2.0f) / sin(theta / 2.0f);

	return Quaternion(imaginary_scale*x, imaginary_scale*y, imaginary_scale*z, cos(theta*other/2.0f));
}

Quaternion Quaternion::Inverse() const {
	return Quaternion(-x, -y, -z, w);
}

bool Quaternion::operator==(const Quaternion& other) const {
	return (FloatsAlmostEqual(x, other.x) && FloatsAlmostEqual(y, other.y) && FloatsAlmostEqual(z, other.z) && FloatsAlmostEqual(w, other.w))
		|| (FloatsAlmostEqual(x, -other.x) && FloatsAlmostEqual(y, -other.y) && FloatsAlmostEqual(z, -other.z) && FloatsAlmostEqual(w, -other.w));
}

Quaternion Quaternion::StripAxis(AxisID axis) const {
	Quaternion new_quaternion = *this;
	if ((axis & AID_X) != 0) {
		new_quaternion.x = 0;
	}
	if ((axis & AID_Y) != 0) {
		new_quaternion.y = 0;
	}
	if ((axis & AID_Z) != 0) {
		new_quaternion.z = 0;
	}
	new_quaternion.NormalizeImaginary();
	new_quaternion.NormalizeAll();
	return new_quaternion;
}

Quaternion Quaternion::RotationAboutAxis(AxisID axis, float angle_in_radians) {
	switch (axis) {
	case AID_X:
		return Quaternion(sin(angle_in_radians / 2.0f), 0, 0, cos(angle_in_radians / 2));
	case AID_Y:
		return Quaternion(0, sin(angle_in_radians / 2.0f), 0, cos(angle_in_radians / 2));
	case AID_Z:
		return Quaternion(0, 0, sin(angle_in_radians / 2.0f), cos(angle_in_radians / 2));
	default:
		std::cerr << "Rotation about combined axis" << std::endl;
		return Quaternion::Identity();
	}
}

Quaternion Quaternion::RotationAboutLocation(Location location, float angle_in_radians) {
	Location modified_location = location * (-sin(angle_in_radians / 2.0f));
	return Quaternion(
		modified_location.location_[0],
		modified_location.location_[1],
		modified_location.location_[2],
		cos(angle_in_radians / 2.0f));
}

Quaternion Quaternion::Slerp(const Quaternion& q0, const Quaternion& q1, float weight) {
	if (q0.dot(q1) >= 0) {
		return q0*((q0.ToPower(-1)*q1).ToPower(weight));
	} else {
		return q0*((q0.ToPower(-1)*(q1*-1)).ToPower(weight));
	}
}

Quaternion Quaternion::RotationBetweenVectors(const std::array<float, 3>& start_vec, const std::array<float, 3>& end_vec, float proportion_of_angle) {
	std::array<float, 3> rotation_vector = cross(start_vec, end_vec);
	if (FloatsAlmostEqual(rotation_vector[0], 0) && FloatsAlmostEqual(rotation_vector[1], 0) && FloatsAlmostEqual(rotation_vector[2], 0)) {
		// The input vectors are some scale of eachother or one is the zero vector
		// If they are a positive scale or one is the 0 vector then the result should be the identity Quaternion
		// Otherwise they are negative scales of each other and the rotation should be a 180 degree rotation through an arbitrary axis
		if (get_same_direction(start_vec, end_vec)) {
			return Quaternion::Identity();
		}
		else {
			return Quaternion::RotationAboutAxis(AID_X, 3.14159265359f);
		}
	}
	rotation_vector = vertex_normalize(rotation_vector);
	float dot_prod = ::dot(vertex_normalize(start_vec), vertex_normalize(end_vec));
	dot_prod = max(min(dot_prod, 1.0f), -1.0f);
	float angle_between = acos(dot_prod) * proportion_of_angle;
	return Quaternion(
		rotation_vector[0] * sin(angle_between / 2.0f),
		rotation_vector[1] * sin(angle_between / 2.0f),
		rotation_vector[2] * sin(angle_between / 2.0f),
		cos(angle_between / 2.0f));
}

Quaternion Quaternion::RotationBetweenLocations(const Location& start_loc, const Location& end_loc, float proportion_of_angle) {
	return RotationBetweenVectors(start_loc.location_, end_loc.location_, proportion_of_angle);
}

Quaternion Quaternion::Identity() {
	return Quaternion(0, 0, 0, 1);
}

void Quaternion::NormalizeImaginary() {
	float magnitude = 0.0f;
	magnitude += x * x;
	magnitude += y * y;
	magnitude += z * z;
	magnitude = sqrtf(magnitude);
	if (magnitude != 0) {
		x = x / magnitude;
		y = y / magnitude;
		z = z / magnitude;
	}
}

void Quaternion::NormalizeAll() {
	float magnitude = 0.0f;
	magnitude += x * x;
	magnitude += y * y;
	magnitude += z * z;
	if (magnitude == 0) {
		w = 1.0f;
		return;
	}
	magnitude += w * w;
	magnitude = sqrtf(magnitude);
	x = x / magnitude;
	y = y / magnitude;
	z = z / magnitude;
	w = w / magnitude;
}

float Quaternion::ImaginaryMagnitude() {
	float magnitude = 0.0f;
	magnitude += x * x;
	magnitude += y * y;
	magnitude += z * z;
	magnitude = sqrtf(magnitude);
	return magnitude / sin(acos(w));
}

float Quaternion::Magnitude() {
	float magnitude = 0.0f;
	magnitude += x * x;
	magnitude += y * y;
	magnitude += z * z;
	magnitude += w * w;
	return sqrt(magnitude);
}
