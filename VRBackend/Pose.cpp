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
