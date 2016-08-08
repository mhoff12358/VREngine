#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "SpecializedActors.h"
#include "BoostPythonWrapper.h"

namespace game_scene {
namespace actors {

class NichijouGraph : public Shmactor {
public:
	void AddedToScene() override;

	object graph_;
};

}  // actors
}  // game_scene