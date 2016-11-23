#pragma once
#include "stdafx.h"
#include "StlHelper.h"
#include "PyScene.h"

#include "SceneSystem/CollisionShape.h"

void CollisionShapeInterface() {
	class_<CollisionShape>("CollisionShape", init<Pose, float>())
		.def(init<Pose, float, bool>())
		.def("Intersect", &CollisionShape::Intersect)
		.def("SetPose", &CollisionShape::SetPose)
		.def("EnDisable", &CollisionShape::EnDisable);

	CreateVector<CollisionShape>("CollisionShape");
}
