#pragma once

#include "stdafx.h"
#include "ActorId.h"

namespace game_scene {
class DLLSTUFF Target
{
public:
	explicit Target(ActorId id);
	~Target();

	static Target AllActors();

	ActorId actor_id_;
};
}  // game_scene