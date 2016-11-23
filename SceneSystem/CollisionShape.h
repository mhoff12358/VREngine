#pragma once
#include "stdafx.h"
#include "VRBackend/Pose.h"

class CollisionShape {
public:
	CollisionShape(Pose pose, float radius, bool enabled = true);

	bool Intersect(const CollisionShape& other) const;
	void SetPose(const Pose& pose);
	void EnDisable(bool enable);

private:
	Location location_;
	float radius_;
	bool enabled_;
};