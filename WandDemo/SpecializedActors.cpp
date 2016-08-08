#include "stdafx.h"

#include "SpecializedActors.h"
#include "SpecializedQueries.h"
#include "Scene.h"

namespace game_scene {
namespace actors {

void MovableCamera::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case commands::InputCommandType::INPUT_UPDATE:
		HandleInput(dynamic_cast<const commands::InputUpdate&>(args));
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

void MovableCamera::HandleInput(const commands::InputUpdate& args) {
	if (args.input_.GetKeyPressed('W')) {
	}
}

void GraphicsResources::AddedToScene() {
	if (scene_->FindByName("GraphicsResources") != ActorId::UnsetId) {
		std::cout << "Attempting to register a second GraphicsResources instance" << std::endl;
		return;
	}
	scene_->RegisterByName("GraphicsResources", id_);
}

void GraphicsResources::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case commands::GraphicsCommandType::REQUIRE_RESOURCE:
		RequireResource(dynamic_cast<const CommandArgsWrapper<ResourceIdent>&>(args).data_);
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

unique_ptr<QueryResult> GraphicsResources::AnswerQuery(const QueryArgs& args) {
	if (args.Type() == queries::GraphicsResourceQueryType::GRAPHICS_RESOURCE_REQUEST) {
		return make_unique<QueryResultWrapped<actors::GraphicsResources&>>(queries::GraphicsResourceQueryType::GRAPHICS_RESOURCE_REQUEST, *this);
	}
	return nullptr;
}

void GraphicsResources::RequireResource(ResourceIdent resource_ident) {
	resource_pool_.PreloadResource(resource_ident);
}

}  // actors
}  // game_scene
