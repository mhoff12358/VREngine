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
	AcceptNewPose(string name, string pose_source, Pose new_pose, Pose old_pose) :
		CommandArgs(PoseableCommand::ACCEPT_NEW_POSE),
		name_(name),
		pose_source_(pose_source),
		new_pose_(new_pose),
		old_pose_(old_pose) {}

	string name_;
	string pose_source_;
	Pose new_pose_;
	Pose old_pose_;
};

}  // commands

namespace actors {

template<typename ActorBase>
class Poseable : public ActorBase {
public:
  struct PoseData {
    enum FreezeBits : unsigned char {
      LOCATION = 0x1,
      ORIENTATION = 0x2,
      SCALE = 0x4,
    };

    PoseData(Pose pose) : PoseData(pose, Pose(), 0x0) {}
    PoseData(Pose pose, Pose freeze_pose, unsigned char freeze_bits)
      : freeze_pose_(freeze_pose_), freeze_bits_(0x0), pose_(ApplyFreezeToPose(pose)) {}

    Pose ApplyFreezeToPose(Pose pose) {
      if (freeze_bits_ & LOCATION) {
        pose.location_ = freeze_pose_.location;
      }
      if (freeze_bits_ & ORIENTATION) {
        pose.orientation_ = freeze_pose_.orientation;
      };
      if (freeze_bits_ & SCALE) {
        pose.scale_ = freeze_pose_.scale;
      }
      return pose;
    }

    Pose freeze_pose_;
    unsigned char freeze_bits_;
    Pose pose_;
  };

	void PushNewPoseImpl(pair<string, Pose> pose) {
		auto existing_pose = stored_poses_.find(std::get<0>(pose));
		if (existing_pose != stored_poses_.end()) {
      PoseData& existing_pose_data = existing_pose->second;
      Pose frozen_input_pose = existing_pose_data.ApplyFreezeToPose(std::get<1>(pose));
			if (!(existing_pose_data.pose_ == frozen_input_pose)) {
				Pose original_pose = existing_pose_data.pose_;
        existing_pose_data.pose_ = frozen_input_pose;
				CommandNewPose(pose.first, existing_pose_data.pose_, original_pose);
			}
		}
		else {
			RegisterNamedPoseImpl(make_pair(pose.first, PoseData(pose, Pose(), 0x0)));
			CommandNewPose(pose.first, pose.second, Pose());
		}
	}

  void FreezePoseImpl(string name, Pose new_pose, unsigned char freeze_bits) {
    auto existing_pose = stored_poses_.find(std::get<0>(pose));
    if (existing_pose != stored_poses_.end()) {
      PoseData& existing_pose_data = existing_pose->second;
      PoseData new_frozen_pose = PoseData(existing_pose_data.pose_, new_pose, freeze_bits);
      if (!(new_frozen_pose.pose_ == existing_pose_data.pose_)) {
        Pose original_pose = existing_pose_data.pose_;
        existing_pose_data = new_frozen_pose;
        CommandNewPose(name, existing_pose_data.pose_, original_pose);
      }
    } else {
      RegisterNamedPoseImpl(make_pair(name, PoseData(Pose(), new_pose, freeze_bits)));
    }
  }

	void RegisterNamedPoseImpl(pair<string, PoseData> pose) {
		stored_poses_[std::get<0>(pose)] = std::get<1>(pose);
	}

	void ClearNamedPoseImpl(string pose_name) {
		stored_poses_.erase(pose_name);
	}

	void CommandNewPose(string name, Pose new_pose, Pose old_pose) {
		HandleCommandVirt(commands::AcceptNewPose(name, GetNameVirt(), new_pose, old_pose));
	}

	static string GetName() {
		return "Poseable-" + ActorBase::GetName();
	}

private:
	map<string, PoseData> stored_poses_;
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

#define RETURN_IF_POSEABLE(type) decltype(actor.PushNewPoseImpl(Pose()), type())

template<typename ActorType>
auto PushNewPoseSpecial(ActorType& actor, pair<string, Pose> pose, special_) -> RETURN_IF_POSEABLE(void) {
	actor.PushNewPoseImpl(pose);
}

template<typename ActorType>
auto PushNewPoseSpecial(ActorType& actor, pair<string, Pose> pose, general_) -> void {

}

template<typename ActorType>
auto FreezePoseSpecial(ActorType& actor, string name, Pose new_pose, unsigned char freeze_bits, special_) -> RETURN_IF_POSEABLE(void) {
	actor.FreezePoseImpl(pose);
}

template<typename ActorType>
auto FreezePoseSpecial(ActorType& actor, string name, Pose new_pose, unsigned char freeze_bits, general_) -> void {

}

template<typename ActorType>
auto RegisterNamedPoseSpecial(ActorType& actor, pair<string, PoseData> pose, special_) -> RETURN_IF_POSEABLE(void) {
	actor.RegisterNamedPoseImpl(pose);
}

template<typename ActorType>
auto RegisterNamedPoseSpecial(ActorType& actor, pair<string, PoseData> pose, general_) -> void {

}

template<typename ActorType>
auto ClearNamedPoseSpecial(ActorType& actor, string pose_name, special_) -> void {
	actor.ClearNamedPoseImpl(pose_name);
}

template<typename ActorType>
auto ClearNamedPoseSpecial(ActorType& actor, string pose_name, general_) -> void {

}

template<typename ActorType>
void PushNewPose(ActorType& actor, pair<string, Pose> pose) {
	PushNewPoseSpecial(actor, pose, special_());
}

template<typename ActorType>
void FreezePose(ActorType& actor, string name, Pose new_pose, unsigned char freeze_bits) {
	PushNewPoseSpecial(actor, name, new_pose, freeze_bits, special_());
}

template<typename ActorType>
void ClearNamedPose(ActorType& actor, string pose_name) {
	ClearNamedPoseSpecial(actor, pose_name, special_());
}

template<typename ActorType>
void RegisterNamedPose(ActorType& actor, pair<string, PoseData> pose) {
	RegisterNamedPoseSpecial(actor, pose, special_());
}
}  // poseable
}  // game_scene
