#include "stdafx.h"
#include "Pose.h"

Location operator*(float scale, const Location& other) {
	return other * scale;
}
