#pragma once

#include "stdafx.h"

template <int width>
array<float, width> Generate1DGausianWithPeak(float peak, float center, float std_dev);
template <int width>
array<float, width> Generate1DGausianWithSum(float sum, float center, float std_dev);

template <int width>
array<float, 4 * (width + 1)> FillHLSLKernel(const array<float, width>& gaussian);

template <int width>
array<float, width> Generate1DGausianWithPeak(float peak, float center, float std_dev) {
	array<float, width> gaussian;
	for (int x = 0; x < width; x++) {
		gaussian[x] = peak * exp(-pow(x - width / 2 - center, 2) / (2 * pow(std_dev, 2)));
	}
	return gaussian;
}

template <int width>
array<float, width> Generate1DGausianWithSum(float sum, float center, float std_dev) {
	array<float, width> gaussian;
	float current_sum = 0;
	for (int x = 0; x < width; x++) {
		gaussian[x] = exp(-pow(x - width / 2 - center, 2) / (2 * pow(std_dev, 2)));
		current_sum += gaussian[x];
	}
	for (int x = 0; x < width; x++) {
		gaussian[x] *= sum / current_sum;
	}
	return gaussian;
}

template <int width>
array<float, 4 * (width + 1)> FillHLSLKernel(const array<float, width>& gaussian) {
	array<float, 4 * (width + 1)> kernel;
	for (int i = 0; i < width; i++) {
		kernel[4 * (i + 1)] = gaussian[i];
	}
	return kernel;
}
