#include "stdafx.h"

#include "GraphicsResources.h"
#include "Scene.h"
#include "GraphicsObject.h"

FORCE_LINK_THIS(GraphicsResources)

namespace game_scene {
	
REGISTER_QUERY(GraphicsResourceQuery, GRAPHICS_RESOURCE_REQUEST);

namespace actors {

void GraphicsResourcesImpl::HandleCommand(CommandArgs& args) {
	switch (args.Type()) {
	case GraphicsObjectCommand::REQUIRE_RESOURCE:
		RequireResource(dynamic_cast<const WrappedCommandArgs<ResourceIdent>&>(args).data_);
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

unique_ptr<QueryResult> GraphicsResourcesImpl::AnswerQuery(const QueryArgs& args) {
	switch (args.Type()) {
	case GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST:
		return make_unique<QueryResultWrapped<actors::GraphicsResources&>>(GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST, dynamic_cast<actors::GraphicsResources&>(*this));
	default:
		return nullptr;
	}
}

void GraphicsResourcesImpl::RequireResource(ResourceIdent resource_ident) {
	resource_pool_.PreloadResource(resource_ident);
}

GraphicsResources& GraphicsResourcesImpl::GetGraphicsResources(Scene* scene) {
	Target graphics_resources_target = Target(scene->FindByName("GraphicsResources"));
	unique_ptr<QueryResult> graphics_resources_result = scene->AskQuery(
		graphics_resources_target,
		QueryArgs(GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST));
	if (graphics_resources_result->Type() != GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST) {
		std::cout << "UNEXPECTED RESPONSE WHILE GETTING GRAPHICS RESOURCES" << std::endl;
	}
	return dynamic_cast<QueryResultWrapped<actors::GraphicsResources&>*>(
		graphics_resources_result.get())->data_;
}

}  // actors
}  // game_scene
