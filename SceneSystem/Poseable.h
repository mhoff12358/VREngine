#pragma once

#include "Actor.h"
#include "VRBackend/Pose.h"

namespace game_scene {
namespace commands {

class PoseableCommand {
public:
	DECLARE_COMMAND(PoseableCommand, ACCEPT_NEW_POSE);
};

class AcceptNewPose : public CommandArgs {
public:
	AcceptNewPose(string name, Pose new_pose, Pose old_pose) :
		CommandArgs(PoseableCommand::ACCEPT_NEW_POSE),
		name_(name),
		new_pose_(new_pose),
		old_pose_(old_pose) {}

	string name_;
	Pose new_pose_;
	Pose old_pose_;
};

}  // commands

namespace actors {

template<typename ActorBase>
class Poseable : public ActorBase {
public:
	void PushNewPoseImpl(pair<string, Pose> pose) {
		auto existing_pose = stored_poses_.find(std::get<0>(pose));
		if (existing_pose != stored_poses_.end()) {
			if (!(existing_pose->second == std::get<1>(pose))) {
				CommandNewPose(pose.first, pose.second, existing_pose->second);
			}
		}
		else {
			CommandNewPose(pose.first, pose.second, Pose());
		}
	}

	void RegisterNamedPoseImpl(pair<string, Pose> pose) {
		stored_poses_.insert(pose);
	}

	void ClearNamedPoseImpl(string pose_name) {
		stored_poses_.erase(pose_name);
	}

	void CommandNewPose(string name, Pose new_pose, Pose old_pose) {
		HandleCommandVirt(commands::AcceptNewPose(name, new_pose, old_pose));
	}

	static string GetName() {
		return "Poseable-" + ActorBase::GetName();
	}

private:
	map<string, Pose> stored_poses_;

};

template<typename ActorBase>
class PrintNewPoses : public ActorBase {
public:
	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case commands::PoseableCommand::ACCEPT_NEW_POSE:
		{
			auto& poses = dynamic_cast<commands::AcceptNewPose&>(args);
			std::cout << "UPDATING POSE: " << poses.name_ << ": OLD POSE: " << std::to_string(poses.old_pose_) << ": NEW POSE: " << std::to_string(poses.new_pose_) << std::endl;
		}
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	static string GetName() {
		return "PrintNewPoses-" + ActorBase::GetName();
	}
};

}  // actors

namespace poseable {
template <typename ActorType, typename OutputType = void>
struct IsPoseable {
	template<typename U, void(U::*)(pair<string, Pose>)> struct SFINAE {};
    template<typename U> static char Test(SFINAE<U, &U::PushNewPoseImpl>*);
    template<typename U> static int Test(...);

	typename typedef std::integral_constant<bool, sizeof(Test<ActorType>(0)) == sizeof(char)> type;
};

struct general_ {};
struct special_ : public general_ {};

template<typename ActorType>
auto PushNewPoseSpecial(ActorType& actor, pair<string, Pose> pose, special_) -> decltype(actor.PushNewPoseImpl(pose), void()) {
	actor.PushNewPoseImpl(pose);
}

template<typename ActorType>
auto PushNewPoseSpecial(ActorType& actor, pair<string, Pose> pose, general_) -> void {

}

template<typename ActorType>
auto RegisterNamedPoseSpecial(ActorType& actor, pair<string, Pose> pose, std::true_type) -> void {
	actor.RegisterNamedPoseImpl(pose);
}

template<typename ActorType>
auto RegisterNamedPoseSpecial(ActorType& actor, pair<string, Pose> pose, std::false_type) -> void {

}

template<typename ActorType>
auto ClearNamedPoseSpecial(ActorType& actor, string pose_name, std::true_type) -> void {
	actor.ClearNamedPoseImpl(pose_name);
}

template<typename ActorType>
auto ClearNamedPoseSpecial(ActorType& actor, string pose_name, std::false_type) -> void {

}

template<typename ActorType>
void PushNewPose(ActorType& actor, pair<string, Pose> pose) {
	PushNewPoseSpecial(actor, pose, special_());
}

template<typename ActorType>
void ClearNamedPose(ActorType& actor, string pose_name) {
	ClearNamedPoseSpecial(actor, pose_name, IsPoseable<ActorType>::type());
}

template<typename ActorType>
void RegisterNamedPose(ActorType& actor, pair<string, Pose> pose) {
	RegisterNamedPoseSpecial(actor, pose, IsPoseable<ActorType>::type());
}
}  // poseable
}  // game_scene
