#pragma once

#include "Actor.h"
#include "VRBackend/Pose.h"

namespace game_scene {
namespace actors {

template<typename ActorBase>
class Pose : public ActorBase {
protected:
	void PushNewPose(Pose pose) {
		AcceptNewPose(pose);
	}

	virtual void AcceptNewPose(const Pose& pose) {}

};

template <typename ActorType, typename OutputType = void>
struct IsPoseable {
	typedef decltype(void(ActorType::*a)(Pose) = ActorType::PushNewPose, OutputType) type;
};

template<typename ActorType>
auto PushNewPose(ActorType& actor, Pose pose) -> IsPoseable<ActorType> {
	actor.PushNewPose(pose);
}

}  // actors
}  // game_scene
