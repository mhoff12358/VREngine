#include "stdafx.h"
#include "NichijouGraph.h"

#include "Scene.h"
#include "QueryArgs.h"
#include "GraphicsObject.h"
#include "HeadsetInterface.h"

namespace game_scene {
namespace actors {

void NichijouGraph::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
		try {
			object main_namespace = import("__main__").attr("__dict__");
			exec("import scripts.nichijou_graph as nichijou_graph", main_namespace);
			object loaded_module = main_namespace["nichijou_graph"];
			graph_ = loaded_module.attr("load_graph")();

			CommandQueueLocation next_command_location = scene_->FrontOfCommands();

			object vertices = graph_["vertices"];
			for (ssize_t i = 0; i < len(vertices); i++) {
				object vertex = vertices[i];
				ActorId new_actor;
				std::tie(new_actor, next_command_location) = scene_->AddActorReturnInitialize(make_unique<NichijouVertex>(vertex, id_), next_command_location);
				vertex_actors_.emplace(string(extract<string>(vertex.attr("name"))), new_actor);
			}

			object edges = graph_["edges"];
			for (ssize_t i = 0; i < len(edges); i++) {
				object edge = edges[i];
				ActorId new_actor;
				std::tie(new_actor, next_command_location) = scene_->AddActorReturnInitialize(make_unique<NichijouEdge>(edge, id_), next_command_location);
				edge_actors_.push_back(new_actor);
			}

			int a = 0;
			for (const pair<string, ActorId>& vertex_pair : vertex_actors_) {
				next_command_location = scene_->MakeCommandAfter(
					next_command_location,
					Command(
						Target(vertex_pair.second),
						make_unique<WrappedCommandArgs<Location>>(
							commands::NichijouGraphCommandType::SET_VERTEX_LOCATION,
							Location(a++, 0, 0))));
			}

			ActorId headset_interface = scene_->FindByName("HeadsetInterface");
			if (headset_interface != ActorId::UnsetId) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(headset_interface),
					make_unique<WrappedCommandArgs<ActorId>>(commands::HeadsetInterfaceCommandType::REGISTER_CONTROLLER_LISTENER, id_)));
			}
		} catch (error_already_set) {
			PyErr_Print();
		}
	}
	break;
	case commands::HeadsetInterfaceCommandType::LISTEN_CONTROLLER_MOVEMENT:
	{
		Location movement = dynamic_cast<const WrappedCommandArgs<Location>&>(args).data_;
		for (const pair<string, ActorId>& vertex_actor : vertex_actors_) {
			scene_->MakeCommandAfter(
				scene_->FrontOfCommands(),
				Command(
					Target(vertex_actor.second),
					make_unique<WrappedCommandArgs<Location>>(commands::NichijouGraphCommandType::MOVE_VERTEX_LOCATION, movement)));
		}
	}
	break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

unique_ptr<QueryResult> NichijouGraph::AnswerQuery(const QueryArgs& args) {
	switch (args.Type()) {
	case queries::NichijouGraphQueryType::GET_VERTEX:
	{
		return make_unique<QueryResultWrapped<ActorId>>(queries::NichijouGraphQueryType::GET_VERTEX, vertex_actors_[dynamic_cast<const WrappedQueryArgs<string>&>(args).data_]);
	}
	default:
		break;
	}
	return make_unique<EmptyQueryResult>();
}

void NichijouVertex::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
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

		CommandQueueLocation last_command;
		std::tie(vertex_graphics_, last_command) = scene_->AddActorReturnInitialize(make_unique<game_scene::actors::GraphicsObject>());
		last_command = scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(vertex_graphics_), 
			make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
				game_scene::commands::GraphicsCommandType::CREATE_COMPONENTS,
				sphere_details)));
		last_command = scene_->MakeCommandAfter(last_command, Command(
			game_scene::Target(vertex_graphics_),
			make_unique<game_scene::commands::ComponentPlacement>(
				"Sphere", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f))));
	}
	break;
	case commands::NichijouGraphCommandType::TELL_VERTEX_ABOUT_EDGE:
	{
		edges_.push_back(dynamic_cast<const WrappedCommandArgs<ActorId>&>(args).data_);
	}
	break;
	case commands::NichijouGraphCommandType::SET_VERTEX_LOCATION:
	{
		location_ = dynamic_cast<const WrappedCommandArgs<Location>&>(args).data_;
		UpdateAllEdgeLocations();
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			game_scene::Target(vertex_graphics_),
			make_unique<game_scene::commands::ComponentPlacement>(
				"Sphere",
				DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * Pose(location_, Quaternion::Identity()).GetMatrix())));
	}
	break;
	case commands::NichijouGraphCommandType::MOVE_VERTEX_LOCATION:
	{
		location_ += dynamic_cast<const WrappedCommandArgs<Location>&>(args).data_;
		UpdateAllEdgeLocations();
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			game_scene::Target(vertex_graphics_),
			make_unique<game_scene::commands::ComponentPlacement>(
				"Sphere",
				DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f) * Pose(location_, Quaternion::Identity()).GetMatrix())));
	}
	break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

void NichijouVertex::UpdateEdgeLocation(ActorId edge_actor) {
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(edge_actor),
		make_unique<WrappedCommandArgs<tuple<ActorId, Location>>>(commands::NichijouGraphCommandType::SET_EDGE_LOCATION, tuple<ActorId, Location>(id_, location_))));
}

void NichijouVertex::UpdateAllEdgeLocations() {
	for (ActorId id : edges_) {
		UpdateEdgeLocation(id);
	}
}

void NichijouEdge::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
		u_actor_ = dynamic_cast<QueryResultWrapped<ActorId>&>(*scene_->AskQuery(Target(graph_), make_unique<WrappedQueryArgs<string>>(queries::NichijouGraphQueryType::GET_VERTEX, string(extract<string>(edge_.attr("u_name")))))).data_;
		v_actor_ = dynamic_cast<QueryResultWrapped<ActorId>&>(*scene_->AskQuery(Target(graph_), make_unique<WrappedQueryArgs<string>>(queries::NichijouGraphQueryType::GET_VERTEX, string(extract<string>(edge_.attr("v_name")))))).data_;
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(u_actor_), make_unique<WrappedCommandArgs<ActorId>>(commands::NichijouGraphCommandType::TELL_VERTEX_ABOUT_EDGE, id_)));
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(v_actor_), make_unique<WrappedCommandArgs<ActorId>>(commands::NichijouGraphCommandType::TELL_VERTEX_ABOUT_EDGE, id_)));
	}
	break;
	case commands::NichijouGraphCommandType::SET_EDGE_LOCATION:
	{
		const tuple<ActorId, Location>& new_location = dynamic_cast<const WrappedCommandArgs<tuple<ActorId, Location>>&>(args).data_;
		if (get<0>(new_location) == u_actor_) {
			u_location_ = get<1>(new_location);
		} else if (get<0>(new_location) == v_actor_) {
			v_location_ = get<1>(new_location);
		}
	}
	break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

}  // actors
}  // game_scene
