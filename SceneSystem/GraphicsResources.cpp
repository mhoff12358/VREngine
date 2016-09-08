#include "stdafx.h"

#include "GraphicsResources.h"
#include "Scene.h"
#include "GraphicsObject.h"

namespace game_scene {
	
REGISTER_QUERY(GraphicsResourceQuery, GRAPHICS_RESOURCE_REQUEST);

namespace actors {

void GraphicsResources::AddedToScene() {
	if (scene_->FindByName("GraphicsResources") != ActorId::UnsetId) {
		std::cout << "Attempting to register a second GraphicsResources instance" << std::endl;
		return;
	}
	scene_->RegisterByName("GraphicsResources", id_);
}

void GraphicsResources::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case GraphicsObjectCommand::REQUIRE_RESOURCE:
		RequireResource(dynamic_cast<const WrappedCommandArgs<ResourceIdent>&>(args).data_);
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

unique_ptr<QueryResult> GraphicsResources::AnswerQuery(const QueryArgs& args) {
	switch (args.Type()) {
	case GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST:
		return make_unique<QueryResultWrapped<actors::GraphicsResources&>>(GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST, *this);
	default:
		return nullptr;
	}
}

void GraphicsResources::RequireResource(ResourceIdent resource_ident) {
	resource_pool_.PreloadResource(resource_ident);
}

GraphicsResources& GraphicsResources::GetGraphicsResources(Scene* scene) {
	Target graphics_resources_target = Target(scene->FindByName("GraphicsResources"));
	unique_ptr<QueryResult> graphics_resources_result = scene->AskQuery(
		graphics_resources_target,
		make_unique<QueryArgs>(
			GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST));
	if (graphics_resources_result->Type() != GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST) {
		std::cout << "UNEXPECTED RESPONSE WHILE GETTING GRAPHICS RESOURCES" << std::endl;
	}
	return dynamic_cast<QueryResultWrapped<actors::GraphicsResources&>*>(
		graphics_resources_result.get())->data_;
}

}  // actors
}  // game_scene
