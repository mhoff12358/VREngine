#include "stdafx.h"
#include "Pose.h"

Location operator*(float scale, const Location& other) {
	return other * scale;
}

bool FloatsAlmostEqual(float a, float b, float tolerance) {
	bool equals = fabs(a - b) <= max(1, max(fabs(a), fabs(b))) * tolerance;
	if (!equals) {
		return false;
	}
	return equals;
}

string std::to_string(const Scale& scale) {
	return "Scale<" + std::to_string(scale[0]) + ", " + std::to_string(scale[1]) + ", " + std::to_string(scale[2]) + ">";
}

string std::to_string(const Location& location) {
	return "Location<" + std::to_string(location[0]) + ", " + std::to_string(location[1]) + ", " + std::to_string(location[2]) + ">";
}

string std::to_string(const Quaternion& quaternion) {
	return "Quaternion<" + std::to_string(quaternion.x) + ", " + std::to_string(quaternion.y) + ", " + std::to_string(quaternion.z) + ", " + std::to_string(quaternion.w) + ">";
}

string std::to_string(const Pose& pose) {
	return "Pose<" + std::to_string(pose.location_) + ", " + std::to_string(pose.orientation_) + ", " + std::to_string(pose.scale_) + ">";
}

Scale::Scale(float x, float y, float z) : scale_({x, y, z}) {}
Scale::Scale(array<float, 3> scale) : scale_(scale) {}
Scale::Scale(float scale) : scale_({scale, scale, scale}) {}
Scale::Scale() : Scale::Scale(1.0f) {}

Scale Scale::operator*(const Scale& other) const { return Scale(scale_[0] * other[0], scale_[1] * other[1], scale_[2] * other[2]); }
bool Scale::operator==(const Scale& other) const { return (scale_[0] == other[0]) && (scale_[1] == other[1]) && (scale_[2] == other[2]); }
float Scale::operator[](size_t index) const { return scale_[index]; }
float& Scale::get(size_t index) { return scale_[index]; }

Scale Scale::Inverse() const {
	return Scale(1 / scale_[0], 1 / scale_[1], 1 / scale_[2]);
}

Location::Location(float x, float y, float z) : location_({x, y, z}) {}
Location::Location(array<float, 3> location) : location_(location) {}
Location::Location() : Location(0.0f, 0.0f, 0.0f) {}

Location Location::Inverse() const { return *this * -1; }
float Location::operator[](size_t index) const { return location_[index]; }
float& Location::get(size_t index) { return location_[index]; }
Location Location::operator+(const Location& other) const { return Location(location_[0] + other[0], location_[1] + other[1], location_[2] + other[2]); }
Location Location::operator-(const Location& other) const { return Location(location_[0] - other[0], location_[1] - other[1], location_[2] - other[2]); }
Location Location::operator*(float scale) const { return Location(scale * location_[0], scale * location_[1], scale * location_[2]); }
Location Location::operator*(const Scale& scale) const { return Location(scale[0] * location_[0], scale[1] * location_[1], scale[2] * location_[2]); }
Location Location::operator/(float scale) const { return *this * (1.0f / scale); }
void Location::operator+=(const Location& other) { for (int i = 0; i < 3; i++) { location_[i] += other.location_[i]; } }
void Location::operator-=(const Location& other) { for (int i = 0; i < 3; i++) { location_[i] -= other.location_[i]; } }
void Location::operator*=(float scale) { for (int i = 0; i < 3; i++) { location_[i] *= scale; } }
void Location::operator/=(float scale) { *this *= 1.0f / scale; }
Location Location::Rotate(const Quaternion& rotation) const { return Location(rotation.ApplyToVector(location_)); }
bool Location::operator==(const Location& other) const { return FloatsAlmostEqual(location_[0], other[0]) && FloatsAlmostEqual(location_[1], other[1]) && FloatsAlmostEqual(location_[2], other[2]); }

float Location::GetLengthSquared() const { return pow(location_[0], 2.0f) + pow(location_[1], 2.0f) + pow(location_[2], 2.0f); }
float Location::GetLength() const { return pow(GetLengthSquared(), 0.5f); }
Location Location::GetNormalized() const { return *this / GetLength(); }

float Location::Dot(const Location& other) const { return (location_[0] * other.location_[0]) + (location_[1] * other.location_[1]) + (location_[2] * other.location_[2]); }


Pose::Pose(Location location, Quaternion orientation, Scale scale) : orientation_(orientation), location_(location), scale_(scale) {}
Pose::Pose(Location location, Quaternion orientation) : orientation_(orientation), location_(location), scale_() {}
Pose::Pose(Location location, Scale scale) : orientation_(), location_(location), scale_(scale) {}
Pose::Pose(Quaternion orientation, Scale scale) : orientation_(orientation), location_(), scale_(scale) {}
Pose::Pose(Location location) : location_(location), orientation_(), scale_() {}
Pose::Pose(Quaternion orientation) : orientation_(orientation), location_(), scale_() {}
Pose::Pose(Scale scale) : orientation_(), location_(), scale_(scale) {}
Pose::Pose() : location_(), orientation_(Quaternion::Identity()), scale_() {}

Pose Pose::ApplyAfter(const Pose& other) const {
	return Pose((location_ * other.scale_).Rotate(other.orientation_) + other.location_, other.orientation_ * orientation_, scale_ * other.scale_);
}

Pose Pose::UnapplyAfter(const Pose& other) const {
	Quaternion inv_rot = orientation_ * other.orientation_.Inverse();
	Scale inv_scale = scale_ * other.scale_.Inverse();
	return Pose(location_ - (other.location_ * inv_scale).Rotate(inv_rot), inv_rot, inv_scale);
}

Pose Pose::Inverse() const {
	return Pose(location_.Inverse(), orientation_.Inverse(), scale_.Inverse());
}

Pose Pose::Delta(const Pose& other) const {
	Quaternion inv = other.orientation_.Inverse() * orientation_;
	return Pose((location_ - other.location_).Rotate(other.orientation_.Inverse()) * other.scale_.Inverse(), inv, scale_ * other.scale_.Inverse());
}

DirectX::XMMATRIX Pose::GetMatrix() const {
	return
		DirectX::XMMatrixScaling(scale_[0], scale_[1], scale_[2]) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(orientation_.x, orientation_.y, orientation_.z, orientation_.w)) *
		DirectX::XMMatrixTranslation(location_[0], location_[1], location_[2]);
}

bool Pose::operator==(const Pose& other) const {
	return (scale_ == other.scale_) && (orientation_ == other.orientation_) && (location_ == other.location_);
}
