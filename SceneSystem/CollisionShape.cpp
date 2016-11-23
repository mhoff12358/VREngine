#include "stdafx.h"
#include "CollisionShape.h"

CollisionShape::CollisionShape(Pose pose, float radius, bool enabled)
	: location_(pose.location_), radius_(radius), enabled_(enabled) {

}

bool CollisionShape::Intersect(const CollisionShape& other) const {
	if (!enabled_ || !other.enabled_) {
		return false;
	}
	float radius_sum = other.radius_ + radius_;
	std::cout << (location_ - other.location_).GetLengthSquared() << std::endl;
	return (location_ - other.location_).GetLengthSquared() <= pow(radius_sum, 2);
}

void CollisionShape::SetPose(const Pose& pose) {
	location_ = pose.location_;
}

void CollisionShape::EnDisable(bool enable) {
	enabled_ = enable;
}