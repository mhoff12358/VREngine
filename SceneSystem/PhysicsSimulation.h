#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "Actor.h"
#include "InputCommandArgs.h"

namespace game_scene {
namespace commands {

class PhysicsSimulationCommand : public CommandType {
public:
	DECLARE_COMMAND(PhysicsSimulationCommand, ADD_RIGID_BODY);
	DECLARE_COMMAND(PhysicsSimulationCommand, REMOVE_RIGID_BODY);
};

class AddRemoveRigidBody : public CommandArgs {
public:
	AddRemoveRigidBody(bullet::RigidBody& rigid_body, bool add_not_remove = true) :
		CommandArgs(add_not_remove ? PhysicsSimulationCommand::ADD_RIGID_BODY : PhysicsSimulationCommand::REMOVE_RIGID_BODY),
		rigid_body_(rigid_body) {}

	bullet::RigidBody& rigid_body_;
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

private:
	bullet::World world_;

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
		case InputCommand::TICK:
			physics_simulation_impl_.HandleTimeTick(dynamic_cast<const commands::TimeTick&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

public:
	PhysicsSimulationImpl physics_simulation_impl_;
};
}  // actors
}  // game_scene
