#pragma once

#include "stdafx.h"
#include "Quaternion.h"

bool FloatsAlmostEqual(float a, float b, float tolerance = 1e-6f);

struct Scale {
	Scale(float x, float y, float z) : scale_({x, y, z}) {}
	Scale(array<float, 3> scale) : scale_(scale) {}
	Scale(float scale) : scale_({scale, scale, scale}) {}
	Scale() : Scale(1.0f) {}

	Scale operator*(const Scale& other) const { return Scale(scale_[0] * other[0], scale_[1] * other[1], scale_[2] * other[2]); }
	bool operator==(const Scale& other) const { return (scale_[0] == other[0]) && (scale_[1] == other[1]) && (scale_[2] == other[2]); }
	float operator[](size_t index) const { return scale_[index]; }
	float& get(size_t index) { return scale_[index]; }

	array<float, 3> scale_;
};

struct Location {
	Location(float x, float y, float z) : location_({x, y, z}) {}
	Location(array<float, 3> location) : location_(location) {}
	Location() : Location(0.0f, 0.0f, 0.0f) {}

	Location Inverse() const { return *this * -1; }
	float operator[](size_t index) const { return location_[index]; }
	float& get(size_t index) { return location_[index]; }
	Location operator+(const Location& other) const { return Location(location_[0] + other[0], location_[1] + other[1], location_[2] + other[2]); }
	Location operator-(const Location& other) const { return Location(location_[0] - other[0], location_[1] - other[1], location_[2] - other[2]); }
	Location operator*(float scale) const { return Location(scale * location_[0], scale * location_[1], scale * location_[2]); }
	Location operator*(Scale scale) const { return Location(scale[0] * location_[0], scale[1] * location_[1], scale[2] * location_[2]); }
	Location operator/(float scale) const { return *this * (1.0f / scale); }
	void operator+=(const Location& other) { for (int i = 0; i < 3; i++) { location_[i] += other.location_[i]; } }
	void operator-=(const Location& other) { for (int i = 0; i < 3; i++) { location_[i] -= other.location_[i]; } }
	void operator*=(float scale) { for (int i = 0; i < 3; i++) { location_[i] *= scale; } }
	void operator/=(float scale) { *this *= 1.0f / scale; }
	Location Rotate(const Quaternion& rotation) const { return Location(rotation.ApplyToVector(location_)); }
	bool operator==(const Location& other) const { return FloatsAlmostEqual(location_[0], other[0]) && FloatsAlmostEqual(location_[1], other[1]) && FloatsAlmostEqual(location_[2], other[2]); }

	float GetLengthSquared() const { return pow(location_[0], 2.0f) + pow(location_[1], 2.0f) + pow(location_[2], 2.0f); }
	float GetLength() const { return pow(GetLengthSquared(), 0.5f); }
	Location GetNormalized() const { return *this / GetLength(); }

	float Dot(const Location& other) const { return (location_[0] * other.location_[0]) + (location_[1] * other.location_[1]) + (location_[2] * other.location_[2]); }

	array<float, 3> location_;
};

Location operator*(float scale, const Location& other);

struct Pose {
	Pose(Location location, Quaternion orientation, Scale scale) : orientation_(orientation), location_(location), scale_(scale) {}
	Pose(Location location, Quaternion orientation) : orientation_(orientation), location_(location), scale_() {}
	Pose(Location location, Scale scale) : orientation_(), location_(location), scale_(scale) {}
	Pose(Quaternion orientation, Scale scale) : orientation_(orientation), location_(), scale_(scale) {}
	Pose(Location location) : location_(location), orientation_(), scale_() {}
	Pose(Quaternion orientation) : orientation_(orientation), location_(), scale_() {}
	Pose(Scale scale) : orientation_(), location_(), scale_(scale) {}
	Pose() : location_(), orientation_(Quaternion::Identity()), scale_() {}

	Pose ApplyAfter(const Pose& other) const {
		assert((scale_[0] == 1) && (scale_[1] == 1) && (scale_[2] == 1));
		return Pose(location_ + (other.location_ * scale_).Rotate(orientation_), orientation_ * other.orientation_, scale_ * other.scale_);
	}

	Pose Inverse() const {
		assert((scale_[0] == 1) && (scale_[1] == 1) && (scale_[2] == 1));
		Quaternion rotation_inverse = orientation_.Inverse();
		return Pose(location_.Inverse().Rotate(rotation_inverse), rotation_inverse);
	}

	Pose Delta(const Pose& other) const {
		return other.ApplyAfter(Inverse());
	}

	DirectX::XMMATRIX GetMatrix() const {
		return
			DirectX::XMMatrixScaling(scale_[0], scale_[1], scale_[2]) *
			DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(orientation_.x, orientation_.y, orientation_.z, orientation_.w)) *
			DirectX::XMMatrixTranslation(location_[0], location_[1], location_[2]);
	}

	bool operator==(const Pose& other) const {
		return (scale_ == other.scale_) && (orientation_ == other.orientation_) && (location_ == other.location_);
	}

	Scale scale_;
	Quaternion orientation_;
	Location location_;
};
