#pragma once

#include "stdafx.h"

namespace game_scene {

enum {
	ALL_ACTORS = 0,
	UNSET_ID = 1,
	FIRST_ID = 2,
};

class ActorId
{
public:
	explicit ActorId(int id);
	ActorId();
	~ActorId();

	static ActorId AllActors;
	static ActorId UnsetId;
	static int NextActorId;
	static ActorId GetNewId();

	bool operator<(const ActorId other) const;
	bool operator==(const ActorId other) const;
	bool operator!=(const ActorId other) const;

	int id_;
};

}  // game_scene