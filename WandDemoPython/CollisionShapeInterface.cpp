#include "stdafx.h"
#include "CollisionShapeInterface.h"

#include "StlHelper.h"
#include "SceneSystem/CollisionShape.h"

void CollisionShapeInterface() {
	class_<CollisionShape>("CollisionShape", init<Location, float>());

	CreateVector<CollisionShape>("CollisionShape");
}
