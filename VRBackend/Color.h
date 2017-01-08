#pragma once

#include "stdafx.h"

class Color {
public:
	Color(float r, float g, float b, float a = 1.0f);
	Color(array<float, 4> rgba);
	Color(array<float, 3> rgb);
	Color();

	Color ScaleRGB(float scale) const;
	Color ScaleRGB() const ;
	Color ApplyOnto(const Color& other) const;

	array<float, 4> rgba_;
};
