#pragma once

#include "stdafx.h"

#include "BulletInternals.h"
#include "PhysXInternal.h"

#include "Actor.h"
#include "InputCommandArgs.h"

#include "PhysicsObject.h"
#include "PhysicsInteractions.h"

namespace game_scene {

class PhysicsSimulationCommand {
public:
	DECLARE_COMMAND(PhysicsSimulationCommand, UPDATE_PHYSICS_OBJECT);
  DECLARE_QUERY(PhysicsSimulationCommand, GET_SCENE);
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

namespace actors {

class PhysicsSimulationImpl {
public:
	PhysicsSimulationImpl();

	void HandleTimeTick(const commands::TimeTick& args);

	void PreTimeTick();
	void PostTimeTick();

	void AddPxRigidActor(PxRigidActor* actor);
	void RemovePxRigidActor(PxRigidActor* actor);

	PhysicsSystem system_;
	PhysicsScene scene_;
};

template<typename ActorBase>
class PhysicsSimulation : public ActorBase {
public:
	PhysicsSimulation() : ActorBase(), physics_simulation_impl_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case PhysicsSimulationCommand::UPDATE_PHYSICS_OBJECT:
		{
			UpdateObject(dynamic_cast<UpdatePhysicsObject&>(args));
		}	break;
		case InputCommand::TICK:
			physics_simulation_impl_.HandleTimeTick(dynamic_cast<const commands::TimeTick&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

  unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) {
    switch (args.Type()) {
    case PhysicsSimulationCommand::GET_SCENE:
      return make_unique<QueryResultWrapped<PxScene*>>(PhysicsSimulationCommand::GET_SCENE, GetPhysicsScene());
    }
	  return make_unique<QueryResult>(QueryType::EMPTY);
  }

	static string GetName() {
		return "PhysicsSimulation-" + ActorBase::GetName();
	}

	PxScene* GetPhysicsScene() {
		return physics_simulation_impl_.scene_.scene_;
	}

	PxPhysics* GetPhysics() {
		return physics_simulation_impl_.system_.physics_;
	}

	const PxMaterial& GetDefaultMaterial() {
		return *physics_simulation_impl_.system_.default_material_;
	}

private:
	void PhysicsObjectIsUpdated(ActorId actor_id) {
		auto raw_result = GetScene().AskQuery(Target(actor_id), GetRigidBodies());
		if (raw_result) {
			if (raw_result->Type() == PhysicsObjectQuery::GET_RIGID_BODIES) {
				GetRigidBodiesResult& result = dynamic_cast<GetRigidBodiesResult&>(*raw_result);
				set<PxRigidActor*> new_bodies;
				for (PxRigidActor* new_body : result.rigid_bodies_) {
					new_bodies.insert(new_body);
				}
				auto existing_bodies = rigid_body_sources_.find(actor_id);
				if (existing_bodies != rigid_body_sources_.end()) {
					for (PxRigidActor* body : existing_bodies->second) {
						if (new_bodies.count(body) == 0) {
							physics_simulation_impl_.RemovePxRigidActor(body);
						}
					}
					for (PxRigidActor* body : new_bodies) {
						if (existing_bodies->second.count(body) == 0) {
							physics_simulation_impl_.AddPxRigidActor(body);
						}
					}
					existing_bodies->second = std::move(new_bodies);
				} else {
					for (PxRigidActor* body : new_bodies) {
						physics_simulation_impl_.AddPxRigidActor(body);
					}
					rigid_body_sources_.insert(make_pair(actor_id, std::move(new_bodies)));
				}
			} else {
				std::cout << "Bad response type for physics object update: " << raw_result->type_.Name() << std::endl;
			}
		} else {
			std::cout << "Failed to get a response" << std::endl;
		}
	}

	void UpdateObject(UpdatePhysicsObject& args) {
		switch (args.update_type_) {
		case UpdatePhysicsObject::ADD:
			AddObject(args.object_id_);
			break;
		case UpdatePhysicsObject::REMOVE:
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
				make_unique<AddUpdatedCallback>(
					callback)));
		PhysicsObjectIsUpdated(actor_id);
	}

	void RemoveObject(ActorId actor_id) {
		auto existing_bodies = rigid_body_sources_.find(actor_id);
		if (existing_bodies != rigid_body_sources_.end()) {
			for (PxRigidActor* body : existing_bodies->second) {
				physics_simulation_impl_.RemovePxRigidActor(body);
			}
			rigid_body_sources_.erase(existing_bodies);
		}
	}

	PhysicsSimulationImpl physics_simulation_impl_;
	map<ActorId, set<PxRigidActor*>> rigid_body_sources_;
};
}  // actors
}  // game_scene
