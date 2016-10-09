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

	static const ActorId AllActors;
	static const ActorId UnsetId;
	static const int FirstId;

	bool operator<(const ActorId other) const;
	bool operator==(const ActorId other) const;
	bool operator!=(const ActorId other) const;

	int id_;
};

}  // game_scene