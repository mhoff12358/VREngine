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
