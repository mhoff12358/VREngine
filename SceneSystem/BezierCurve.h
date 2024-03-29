#pragma once
#include "stdafx.h"
#include "VRBackend/Pose.h"

class BezierCurve {
public:
	BezierCurve(vector<Location> control_points);

	Location At(float t);
	Location EvaluateAtT(float t);

	// This returns a pair (sample, distance_squared)
	// If return_distance_squared is false, then the second value is NaN
	pair<float, float> FindNearset(const Location& location, bool return_distance_squared);

	static unsigned int BinomialCoefficient(unsigned int n, unsigned int i);
	static unsigned int RawBinomialCoefficient(unsigned int n, unsigned int i);
	static const array<array<unsigned int, 7>, 7> binomial_coefficient_lookup;

private:
	vector<Location> control_points_;
};

// TODO: Add short circuit bezier curves for small numbers of control points that don't need to calculate constants.