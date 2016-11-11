#include "stdafx.h"
#include "CollisionShape.h"

CollisionShape::CollisionShape(Location location, float radius)
	: location_(location), radius_(radius) {

}

bool CollisionShape::Intersect(const CollisionShape& other) const {
	float radius_sum = other.radius_ + radius_;
	std::cout << (location_ - other.location_).GetLengthSquared() << std::endl;
	return (location_ - other.location_).GetLengthSquared() <= pow(radius_sum, 2);
}
