#include "stdafx.h"

#include "Color.h"

Color::Color(float r, float g, float b, float a) : rgba_({ r, g, b, a }) {
}

Color::Color(array<float, 4> rgba) : rgba_(rgba) {
}

Color::Color(array<float, 3> rgb) : rgba_({ rgb[0], rgb[1], rgb[2], 1.0f }) {
}

Color::Color() : rgba_({ 0, 0, 0, 1 }) {
}

Color Color::ScaleRGB(float scale) const {
	return Color(rgba_[0] * scale, rgba_[1] * scale, rgba_[2] * scale, rgba_[3]);
}

Color Color::ScaleRGB() const {
	Color scaled = ScaleRGB(rgba_[3]);
	scaled.rgba_[3] = 1.0f;
	return scaled;
}

Color Color::ApplyOnto(const Color& other) const {
	Color self_scaled;
	if (other.rgba_[3] == 0) {
		self_scaled = ScaleRGB(rgba_[3]);
	} else {
		self_scaled = ScaleRGB(rgba_[3] / other.rgba_[3]);
	}
	for (size_t i = 0; i < 3; i++) {
		self_scaled.rgba_[i] += other.rgba_[i];
	}
	self_scaled.rgba_[3] = other.rgba_[3];
	return self_scaled;
}