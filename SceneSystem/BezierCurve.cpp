#include "stdafx.h"
#include "BezierCurve.h"

BezierCurve::BezierCurve(vector<Location> control_points)
	: control_points_(std::move(control_points)) {}

Location BezierCurve::At(float t) {
	const int n = control_points_.size();
	Location result = Location(0, 0, 0);
	for (int index = 0; index < n; index++) {
		result += (pow(t, n - index) * pow(1 - t, index) * BinomialCoefficient(n, index)) * control_points_[index];
	}
	return result;
}

Location BezierCurve::EvaluateAtT(float t) {
	return At(t);
}

pair<float, float> FindNearset(const Location& location) {
	return make_pair(0, 0);
}

const array<array<unsigned int, 7>, 7> BezierCurve::binomial_coefficient_lookup = { {
	{1, 1, 1, 1, 1, 1, 1},
	{1, 1, 1, 1, 1, 1, 1},
	{1, 2, 1, 1, 1, 1, 1},
	{1, 3, 3, 1, 1, 1, 1},
	{1, 4, 6, 4, 1, 1, 1},
	{1, 5, 10, 10, 5, 1, 1},
	{1, 6, 15, 20, 15, 6, 1},
} };

unsigned int BezierCurve::BinomialCoefficient(unsigned int n, unsigned int i) {
	return n <= 6 ? (i <= 6 ? binomial_coefficient_lookup[n][i] : RawBinomialCoefficient(n, i)) : RawBinomialCoefficient(n, i);
}

unsigned int BezierCurve::RawBinomialCoefficient(unsigned int n, unsigned int i) {
	unsigned int min_stop = min(n, i);
	unsigned int max_stop = max(n, i);
	unsigned int factorial = 1;
	if (min_stop == 0) {
		return 1;
	}
	for (unsigned int j = 1; j <= min_stop; j++) {
		factorial *= j;
	}
	unsigned int min_factorial = factorial;
	for (unsigned int j = min_stop + 1; j <= max_stop; j++) {
		factorial *= j;
	}
	unsigned int max_factorial = factorial;
	for (unsigned int j = max_stop + 1; j <= n; j++) {
		factorial *= j;
	}
	return factorial / min_factorial / max_factorial;
}