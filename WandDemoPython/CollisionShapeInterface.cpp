#include "stdafx.h"
#include "CollisionShapeInterface.h"

#include "StlHelper.h"
#include "SceneSystem/CollisionShape.h"

void CollisionShapeInterface() {
	bool (CollisionShape::*a)(const CollisionShape&)const = &CollisionShape::Intersect;
	bool (CollisionShape::*b)(const vector<CollisionShape>&)const = &CollisionShape::Intersect;
	class_<CollisionShape, boost::noncopyable>("CollisionShape", init<Pose, float>())
		.def("Intersect", a)
		.def("Intersect", b)
		.def("SetPose", &CollisionShape::SetPose)
		.def("EnDisable", &CollisionShape::EnDisable);

	class_<ColoredCollisionShape, bases<CollisionShape>, boost::noncopyable>("ColoredCollisionShape", init<int, set<int>, Pose, float>())
		.def("Intersect", &ColoredCollisionShape::Intersect);

	CreateVector<CollisionShape>("CollisionShape");
	CreateVector<ColoredCollisionShape>("ColoredCollisionShape");
}
