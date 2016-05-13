#include "stdafx.h"
#include "ActorId.h"

namespace game_scene {
ActorId::ActorId(int id)
	: id_(id)
{
}

ActorId::~ActorId()
{
}

bool ActorId::operator<(const ActorId other) const {
	return id_ < other.id_;
}

bool ActorId::operator==(const ActorId other) const {
	return id_ == other.id_;
}

bool ActorId::operator!=(const ActorId other) const {
	return !(*this == other);
}

ActorId ActorId::AllActors(ALL_ACTORS);
ActorId ActorId::UnsetId(UNSET_ID);
int ActorId::NextActorId = FIRST_ID;
ActorId ActorId::GetNewId() {
	return ActorId(NextActorId++);
}

}  // game_scene