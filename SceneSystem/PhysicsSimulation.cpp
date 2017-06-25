#include "stdafx.h"

#include "PhysicsSimulation.h"

FORCE_LINK_THIS(PhysicsSimulation)

namespace game_scene {

REGISTER_COMMAND(PhysicsSimulationCommand, UPDATE_PHYSICS_OBJECT);
REGISTER_QUERY(PhysicsSimulationCommand, GET_SCENE);

namespace actors {

PhysicsSimulationImpl::PhysicsSimulationImpl() : system_(), scene_(system_) {}

void PhysicsSimulationImpl::HandleTimeTick(const commands::TimeTick& args) {
	PreTimeTick();
	scene_.Step(args.duration_ / 1000.0f);
	PostTimeTick();
}

void PhysicsSimulationImpl::PreTimeTick() {
}

void PhysicsSimulationImpl::PostTimeTick() {
}

void PhysicsSimulationImpl::AddPxRigidActor(PxRigidActor* actor) {
	scene_.scene_->addActor(*actor);
}

void PhysicsSimulationImpl::RemovePxRigidActor(PxRigidActor* actor) {
	scene_.scene_->removeActor(*actor);
}


}  // actors
}  // game_scene
