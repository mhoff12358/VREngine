#pragma once
#include "stdafx.h"
#include "VRBackend/Pose.h"

class CollisionShape {
public:
	CollisionShape(Location location, float radius);

	bool Intersect(const CollisionShape& other) const;

private:
	Location location_;
	float radius_;
};