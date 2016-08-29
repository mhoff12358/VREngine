#include "stdafx.h"
#include "Target.h"

namespace game_scene {
Target::Target(ActorId id) : actor_id_(id) {
}

Target::~Target() {
}

Target Target::AllActors() {
	return Target(ActorId::AllActors);
}

}  // game_scene