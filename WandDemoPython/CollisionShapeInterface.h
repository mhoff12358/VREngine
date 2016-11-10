#pragma once
#include "stdafx.h"
#include "StlHelper.h"
#include "PyScene.h"

#include "SceneSystem/CollisionShape.h"

void CollisionShapeInterface() {
	class_<CollisionShape>("CollisionShape", init<Location, float>());

	CreateVector<CollisionShape>("CollisionShape");
}
