#pragma once

#include "stdafx.h"
#include "Quaternion.h"

bool FloatsAlmostEqual(float a, float b, float tolerance = 1e-6f);

struct Scale {
	Scale(float x, float y, float z);
	explicit Scale(array<float, 3> scale);
	explicit Scale(float scale);
	Scale();

	Scale operator*(const Scale& other) const;
	bool operator==(const Scale& other) const;
	float operator[](size_t index) const;
	float& get(size_t index);

	Scale Inverse() const;

	array<float, 3> scale_;
};

struct Location {
	Location(float x, float y, float z);
	explicit Location(array<float, 3> location);
	Location();

	Location Inverse() const;
	float operator[](size_t index) const;
	float& get(size_t index);
	Location operator+(const Location& other) const;
	Location operator-(const Location& other) const;
	Location operator*(float scale) const;
	Location operator*(const Scale& scale) const;
	Location operator/(float scale) const;
	void operator+=(const Location& other);
	void operator-=(const Location& other);
	void operator*=(float scale);
	void operator/=(float scale);
	Location Rotate(const Quaternion& rotation) const;
	bool operator==(const Location& other) const;

	float GetLengthSquared() const;
	float GetLength() const;
	Location GetNormalized() const;

	Location Cross(const Location& other) const;
	float Dot(const Location& other) const;

	array<float, 3> location_;
};

Location operator*(float scale, const Location& other);

struct Pose {
	Pose(Location location, Quaternion orientation, Scale scale);
	Pose(Location location, Quaternion orientation);
	Pose(Location location, Scale scale);
	Pose(Quaternion orientation, Scale scale);
	explicit Pose(Location location);
	explicit Pose(Quaternion orientation);
	explicit Pose(Scale scale);
	Pose();

	Pose WithoutLocation() const;
	Pose WithoutOrientation() const;
	Pose WithoutScale() const;

	Pose ApplyAfter(const Pose& other) const;
	Pose UnapplyAfter(const Pose& other) const;

	Pose Inverse() const;

	Pose Delta(const Pose& other) const;

	DirectX::XMMATRIX GetMatrix() const;

	bool operator==(const Pose& other) const;

	Scale scale_;
	Quaternion orientation_;
	Location location_;
};

namespace std {
	string to_string(const Scale& scale);
	string to_string(const Location& location);
	string to_string(const Quaternion& quaternion);
	string to_string(const Pose& pose);
}