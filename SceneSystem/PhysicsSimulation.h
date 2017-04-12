#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "Actor.h"
#include "InputCommandArgs.h"

#include "PhysicsObject.h"

namespace game_scene {
namespace commands {

class PhysicsSimulationCommand {
public:
	DECLARE_COMMAND(PhysicsSimulationCommand, UPDATE_PHYSICS_OBJECT);
};

class UpdatePhysicsObject : public CommandArgs {
public:
	enum UpdateType {
		ADD,
		REMOVE,
	};
	UpdatePhysicsObject(UpdateType update_type, ActorId object_id) :
		CommandArgs(PhysicsSimulationCommand::UPDATE_PHYSICS_OBJECT),
		update_type_(update_type),
		object_id_(object_id) {}

	UpdateType update_type_;
	ActorId object_id_;
};

}  // commands

namespace actors {

class PhysicsSimulationImpl {
public:
	PhysicsSimulationImpl(bullet::World world);

	void HandleTimeTick(const commands::TimeTick& args);

	void MakeCallbackOwner();

	void PreTimeTick();
	void PostTimeTick();

	bullet::World world_;

private:
	map<bullet::CollisionInfo::first_type, bullet::CollisionInfo::second_type> current_collisions_;

	static PhysicsSimulationImpl* callback_owner;
	bool ContactProcessedCallback(
		btManifoldPoint& cp,
		void* body0, void* body1);

	bool ContactAddedCallback(
		btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1, int index1);

	static bool ContactProcessedCallbackDelegater(
		btManifoldPoint& cp,
		void* body0, void* body1);

	static bool ContactAddedCallbackDelegater(
		btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1, int index1);
};

template<typename ActorBase>
class PhysicsSimulation : public ActorBase {
public:
	PhysicsSimulation() : ActorBase(), physics_simulation_impl_(bullet::World(bullet::Config())) {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case commands::PhysicsSimulationCommand::UPDATE_PHYSICS_OBJECT:
		{
			auto args = dynamic_cast<commands::UpdatePhysicsObject&>(args);
			switch (args.update_type_) {
			case commands::UpdatePhysicsObject::ADD:
				AddObject(args.object_);
				break;
			case commands::UpdatePhysicsObject::REMOVE:
				RemoveObject(args.object_);
				break;
			}
		}	break;
		case InputCommand::TICK:
			physics_simulation_impl_.HandleTimeTick(dynamic_cast<const commands::TimeTick&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	static string GetName() {
		return "PhysicsSimulation-" + ActorBase::GetName();
	}

private:
	void PhysicsObjectIsUpdated(ActorId actor_id) {
		auto result = dynamic_cast<queries::GetRigidBodiesResult>(
			GetScene().AskQuery(Target(actor_id), queries::GetRigidBodies));
		auto existing_body = rigid_body_sources_.find(actor_id);
		if (result) {
			const bullet::RigidBody& new_rigid_body = result.rigid_body_;
			if (existing_body != rigid_body_sources_.end()) {
				physics_simulation_impl_.world_.RemoveRigidBody(*existing_body);
				*existing_body = new_rigid_body;
				if (new_rigid_body.GetFilled()) {
					physics_simulation_impl_.world_.AddRigidBody(*existing_body);
				}
			} else {
				rigid_body_sources_.insert(make_pair(actor_id, result.rigid_body_));
			}
		}
		else {
			std::cout << "Failed to get a response" << std::endl;
		}
	}

	void AddObject(ActorId actor_id) {
		GetScene().MakeCommandAfter(
			GetScene().BackOfNewCommands(),
			Command(
				Target(actor_id),
				make_unique<commands::AddUpdatedCallback>(
					std::bind(&PhysicsObjectIsUpdated, this))));
		PhysicsObjectIsUpdated(actor_id);
	}

	void RemoveObject(ActorId actor_id) {
		auto existing_body = rigid_body_sources_.find(actor_id);
		if (existing_body != rigid_body_sources_.end()) {
			physics_simulation_impl_.world_.RemoveRigidBody(*existing_body);
			rigid_body_sources_.erase(existing_body);
		}
	}

	PhysicsSimulationImpl physics_simulation_impl_;
	map<ActorId, const bullet::RigidBody&> rigid_body_sources_;
};
}  // actors
}  // game_scene
