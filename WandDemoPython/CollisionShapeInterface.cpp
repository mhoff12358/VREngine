#include "stdafx.h"
#include "CollisionShapeInterface.h"

#include "StlHelper.h"
#include "SceneSystem/CollisionShape.h"

void CollisionShapeInterface() {
	class_<CollisionShape, boost::noncopyable>("CollisionShape", init<Pose, float>())
		.def("Intersect", &CollisionShape::Intersect)
		.def("SetPose", &CollisionShape::SetPose)
		.def("EnDisable", &CollisionShape::EnDisable);

	CreateVector<CollisionShape>("CollisionShape");
}
