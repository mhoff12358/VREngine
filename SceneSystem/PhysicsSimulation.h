#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "Actor.h"
#include "InputCommandArgs.h"

#include "PhysicsObject.h"
#include "PhysicsInteractions.h"

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
			UpdateObject(dynamic_cast<commands::UpdatePhysicsObject&>(args));
		}	break;
		case InputCommand::TICK:
			physics_simulation_impl_.HandleTimeTick(dynamic_cast<const commands::TimeTick&>(args));
			for (auto rigid_body_source : rigid_body_sources_) {
				ActorId actor_to_alert;
				std::tie(actor_to_alert, std::ignore) = rigid_body_source;
				GetScene().MakeCommandAfter(
					GetScene().BackOfNewCommands(),
					Command(
						game_scene::Target(actor_to_alert),
						make_unique<commands::RigidBodyUpdated>()));
			}
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
		auto raw_result = GetScene().AskQuery(Target(actor_id), queries::GetRigidBodies());
		if (raw_result) {
			if (raw_result->Type() == queries::PhysicsObjectQuery::GET_RIGID_BODIES) {
				queries::GetRigidBodiesResult& result = dynamic_cast<queries::GetRigidBodiesResult&>(*raw_result);
				set<const bullet::RigidBody*> new_bodies(result.rigid_bodies_.begin(), result.rigid_bodies_.end());
				auto existing_bodies = rigid_body_sources_.find(actor_id);
				if (existing_bodies != rigid_body_sources_.end()) {
					for (const bullet::RigidBody* body : existing_bodies->second) {
						if (new_bodies.count(body) == 0) {
							physics_simulation_impl_.world_.RemoveRigidBody(*body);
						}
					}
					for (const bullet::RigidBody* body : new_bodies) {
						if (body->GetFilled() && (existing_bodies->second.count(body) == 0)) {
							physics_simulation_impl_.world_.AddRigidBody(*body);
						}
					}
					existing_bodies->second = std::move(new_bodies);
				}
				else {
					rigid_body_sources_.insert(make_pair(actor_id, std::move(new_bodies)));
				}
			} else {
				std::cout << "Bad response type for physics object update: " << raw_result->type_.Name() << std::endl;
			}
		} else {
			std::cout << "Failed to get a response" << std::endl;
		}
	}

	void UpdateObject(commands::UpdatePhysicsObject& args) {
		switch (args.update_type_) {
		case commands::UpdatePhysicsObject::ADD:
			AddObject(args.object_id_);
			break;
		case commands::UpdatePhysicsObject::REMOVE:
			RemoveObject(args.object_id_);
			break;
		}
	}

	void AddObject(ActorId actor_id) {
		game_scene::ActorCallback callback([this](ActorId actor_id) {PhysicsSimulation::PhysicsObjectIsUpdated(actor_id);});
		GetScene().MakeCommandAfter(
			GetScene().BackOfNewCommands(),
			Command(
				Target(actor_id),
				make_unique<commands::AddUpdatedCallback>(
					callback)));
		PhysicsObjectIsUpdated(actor_id);
	}

	void RemoveObject(ActorId actor_id) {
		auto existing_bodies = rigid_body_sources_.find(actor_id);
		if (existing_bodies != rigid_body_sources_.end()) {
			for (const bullet::RigidBody* body : existing_bodies->second) {
				physics_simulation_impl_.world_.RemoveRigidBody(*body);
			}
			rigid_body_sources_.erase(existing_bodies);
		}
	}

	PhysicsSimulationImpl physics_simulation_impl_;
	map<ActorId, set<const bullet::RigidBody*>> rigid_body_sources_;
};
}  // actors
}  // game_scene
