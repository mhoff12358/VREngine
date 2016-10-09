#include "stdafx.h"
#include "ActorId.h"

namespace game_scene {
ActorId::ActorId(int id)
	: id_(id)
{
}

ActorId::ActorId() : id_(UNSET_ID)
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

const ActorId ActorId::AllActors(ALL_ACTORS);
const ActorId ActorId::UnsetId(UNSET_ID);
const int ActorId::FirstId = FIRST_ID;

}  // game_scene