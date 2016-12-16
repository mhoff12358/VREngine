#include "stdafx.h"
#include "CollisionShape.h"

CollisionShape::CollisionShape(Pose pose, float radius, bool enabled)
	: pose_(pose), radius_(radius), enabled_(enabled) {

}

bool CollisionShape::Intersect(const CollisionShape& other) const {
	if (!enabled_ || !other.enabled_) {
		return false;
	}
	float radius_sum = other.radius_ + radius_;
	return (pose_.location_ - other.pose_.location_).GetLengthSquared() <= pow(radius_sum, 2);
}

bool CollisionShape::Intersect(const vector<CollisionShape>& others) const {
	for (const CollisionShape& other : others) {
		if (Intersect(other)) {
			return true;
		}
	}
	return false;
}

void CollisionShape::SetPose(const Pose& pose) {
	pose_ = pose;
}

void CollisionShape::EnDisable(bool enable) {
	enabled_ = enable;
}

Pose CollisionShape::GetPose() const {
	return pose_;
}

ColoredCollisionShape::ColoredCollisionShape(int color, set<int> accepted_colors, Pose pose, float radius, bool enabled) :
	CollisionShape(pose, radius, enabled),
	color_(color),
	accepted_colors_(accepted_colors) {

}

bool ColoredCollisionShape::Intersect(const CollisionShape& other) const {
	const ColoredCollisionShape* colored_other = dynamic_cast<const ColoredCollisionShape*>(&other);
	// Only do a coloration check if the other collision shape is colored.
	if (colored_other) {
		// If there is no overlap in accepted colors return false preemptively before doing an actual intersection check.
		if ((accepted_colors_.find(colored_other->color_) == accepted_colors_.end()) &&
			(colored_other->accepted_colors_.find(color_) == colored_other->accepted_colors_.end())) {
			return false;
		}
	}
	return CollisionShape::Intersect(other);
}