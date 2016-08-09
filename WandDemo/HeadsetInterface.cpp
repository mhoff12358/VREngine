#include "stdafx.h"

#include "HeadsetInterface.h"
#include "SpecializedCommandArgs.h"
#include "GraphicsObject.h"
#include "Scene.h"
#include "SpecializedQueries.h"

namespace game_scene {
namespace actors {

void HeadsetInterface::AddedToScene() {
	if (scene_->FindByName("HeadsetInterface") != ActorId::UnsetId) {
		std::cout << "Attempting to register a second HeadsetInterface instance" << std::endl;
		return;
	}
	scene_->RegisterByName("HeadsetInterface", id_);

	controller_graphics_[0] = CreateControllerActor();
	controller_graphics_[1] = CreateControllerActor();
}

void HeadsetInterface::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
		controller_listeners_ = scene_->AddActorGroup();
	}
	break;
	case commands::HeadsetInterfaceCommandType::REGISTER_CONTROLLER_LISTENER:
	{
		scene_->AddActorToGroup(dynamic_cast<const WrappedCommandArgs<ActorId>&>(args).data_, controller_listeners_);
	}
	break;
	case commands::InputCommandType::TICK:
		for (unsigned int i = 0; i < 2; i++) {
			vr::TrackedDeviceIndex_t controller_index = headset_.GetDeviceIndex(vr::TrackedDeviceClass_Controller, i);
			if (controller_index == vr::k_unTrackedDeviceIndexInvalid) {
				if (controller_connectedness_[i]) {
					// Should make the actor invisible here.
				}
				controller_connectedness_[i] = false;
				continue;
			}
			if (!controller_connectedness_[i]) {
				// Should make the actor visible here.
			}
			controller_connectedness_[i] = true;
			Pose new_controller_position = headset_.GetGamePose(controller_index);
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(controller_listeners_),
				make_unique<WrappedCommandArgs<Location>>(
					commands::HeadsetInterfaceCommandType::LISTEN_CONTROLLER_MOVEMENT,
					new_controller_position.location_ + (controller_positions_[i].location_ * -1))));
			controller_positions_[i] = new_controller_position;
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(controller_graphics_[i]),
				make_unique<commands::ComponentPlacement>("Sphere", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f)*controller_positions_[i].GetMatrix())));
		}
	}
}

ActorId HeadsetInterface::CreateControllerActor() {
	game_scene::actors::GraphicsObjectDetails sphere_details;
	sphere_details.heirarchy_ = game_scene::actors::ComponentHeirarchy(
		"sphere",
		{
			{"sphere.obj|Sphere",
			ObjLoader::OutputFormat(
				ModelModifier(
					{0, 1, 2},
					{1, 1, 1},
					{false, true}),
				ObjLoader::vertex_type_all,
				false)}
		},
		{});
	sphere_details.heirarchy_.shader_name_ = "solidcolor.hlsl";
	sphere_details.heirarchy_.entity_group_ = "basic";
	sphere_details.heirarchy_.shader_settings_ = {
		{0.0f, 0.0f, 0.5f},
	};

	ActorId new_controller = scene_->AddActor(make_unique<game_scene::actors::GraphicsObject>());
	CommandQueueLocation created_actor =
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(new_controller), 
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::commands::GraphicsCommandType::CREATE_COMPONENTS,
			sphere_details)));
	scene_->MakeCommandAfter(created_actor, Command(
		game_scene::Target(new_controller),
		make_unique<game_scene::commands::ComponentPlacement>("Sphere", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f))));
	return new_controller;
}

}  // actors
}  // game_scene
