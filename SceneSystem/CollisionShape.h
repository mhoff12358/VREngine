#pragma once
#include "stdafx.h"
#include "VRBackend/Pose.h"

class CollisionShape {
public:
	CollisionShape(Pose pose, float radius, bool enabled = true);
	virtual ~CollisionShape() {}

	virtual bool Intersect(const CollisionShape& other) const;
	bool Intersect(const vector<CollisionShape>& others) const;
	void SetPose(const Pose& pose);
	void EnDisable(bool enable);

private:
	Location location_;
	float radius_;
	bool enabled_;
};

class ColoredCollisionShape : public CollisionShape {
public:
	ColoredCollisionShape(int color, set<int> accepted_colors, Pose pose, float radius, bool enabled = true);

	bool Intersect(const CollisionShape& other) const override;

private:
	int color_;
	set<int> accepted_colors_;
};