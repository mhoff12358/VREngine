#include "stdafx.h"

#include "PhysicsSimulation.h"

FORCE_LINK_THIS(PhysicsSimulation)

namespace game_scene {
namespace commands {
REGISTER_COMMAND(PhysicsSimulationCommand, UPDATE_PHYSICS_OBJECT);
}  // commands

namespace actors {

PhysicsSimulationImpl::PhysicsSimulationImpl(bullet::World world) : world_(std::move(world)) {}

void PhysicsSimulationImpl::HandleTimeTick(const commands::TimeTick& args) {
	world_.Step(args.duration_ / 1000.0f);
}

void PhysicsSimulationImpl::PreTimeTick() {
	current_collisions_.clear();
	MakeCallbackOwner();
}

void PhysicsSimulationImpl::PostTimeTick() {
	for (const bullet::CollisionInfo& collision_info : current_collisions_) {
		if (collision_info.first.actor_ != ActorId::UnsetId) {
		}
	}
}

PhysicsSimulationImpl* PhysicsSimulationImpl::callback_owner = nullptr;

void PhysicsSimulationImpl::MakeCallbackOwner() {
	callback_owner = this;

	gContactProcessedCallback = ContactProcessedCallbackDelegater;
	gContactAddedCallback = ContactAddedCallbackDelegater;
}

bool PhysicsSimulationImpl::ContactProcessedCallback(
	btManifoldPoint& cp,
	void* body0, void* body1) {
	std::cout << "PROCESSED" << std::endl;
	btCollisionObjectWrapper* colObj0Wrap = static_cast<btCollisionObjectWrapper*>(body0);
	btCollisionObjectWrapper* colObj1Wrap = static_cast<btCollisionObjectWrapper*>(body1);
	return true;
}

bool PhysicsSimulationImpl::ContactAddedCallback(
	btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap,
	int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap,
	int partId1, int index1) {
	std::cout << "ADDED" << std::endl;
	return true;
}

bool PhysicsSimulationImpl::ContactProcessedCallbackDelegater(
	btManifoldPoint& cp,
	void* body0, void* body1) {
	return callback_owner->ContactProcessedCallback(cp, body0, body1);
}

bool PhysicsSimulationImpl::ContactAddedCallbackDelegater(
	btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap,
	int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap,
	int partId1, int index1) {
	return callback_owner->ContactAddedCallback(
		cp,
		colObj0Wrap, partId0, index0,
		colObj1Wrap, partId1, index1);
}

}  // actors
}  // game_scene
