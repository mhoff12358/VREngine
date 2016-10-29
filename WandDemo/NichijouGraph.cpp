#include "stdafx.h"
#include "NichijouGraph.h"

#include "SceneSystem/Scene.h"
#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/GraphicsObject.h"
#include "SceneSystem/GraphicsResources.h"
#include "SceneSystem/HeadsetInterface.h"
#include "SceneSystem/Sprite.h"
#include "SceneSystem/Registry.h"
#include "VRBackend/ResourceIdentifier.h"

#include <cstdlib>

namespace game_scene {

REGISTER_COMMAND(NichijouCommand, TELL_VERTEX_ABOUT_EDGE);
REGISTER_COMMAND(NichijouCommand, SET_VERTEX_LOCATION);
REGISTER_COMMAND(NichijouCommand, MOVE_VERTEX_LOCATION);
REGISTER_COMMAND(NichijouCommand, SET_EDGE_LOCATION);
REGISTER_COMMAND(NichijouCommand, SET_VISIBLE);
REGISTER_COMMAND(NichijouCommand, TELL_TIMELINE_POSITION);

REGISTER_QUERY(NichijouQuery, GET_VERTEX);

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

			configuration_ = graph_["configuration"];

			object vertices = graph_["vertices"];
			object vertex_configuration = configuration_["vertex_configuration"];
			for (ssize_t i = 0; i < len(vertices); i++) {
				object vertex = vertices[i];
				ActorId new_actor;
				std::tie(new_actor, next_command_location) = scene_->AddActorReturnInitialize(make_unique<NichijouVertex>(vertex_configuration, vertex, id_), next_command_location);
				vertex_actors_.emplace(string(extract<string>(vertex.attr("name"))), new_actor);
			}

			object edges = graph_["edges"];
			object edge_configuration = configuration_["edge_configuration"];
			edge_model_name_ = ResourceIdentifier::GetNewModelName("EdgeLine");
			//edge_model_name_ = "Line.obj";
			for (ssize_t i = 0; i < len(edges); i++) {
				object edge = edges[i];
				ActorId new_actor;
				std::tie(new_actor, next_command_location) =
					scene_->AddActorReturnInitialize(
						make_unique<NichijouEdge>(edge_configuration, edge, id_, edge_model_name_),
						next_command_location);
				edge_actors_.push_back(new_actor);
			}

			std::srand(0);
			float graph_radius = extract<float>(configuration_["graph_radius"]);
			for (const pair<string, ActorId>& vertex_pair : vertex_actors_) {
				Location vertex_location;
				float length;
				do {
					vertex_location = {
						static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
						static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
						static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f};
					length = vertex_location.GetLength();
				} while (length == 0.0f || length > 1.0f);
				vertex_location = vertex_location.GetNormalized() * graph_radius;
				next_command_location = scene_->MakeCommandAfter(
					next_command_location,
					Command(
						Target(vertex_pair.second),
						make_unique<WrappedCommandArgs<Location>>(
							NichijouCommand::SET_VERTEX_LOCATION,
							vertex_location)));
			}

			object timeline_length = configuration_["timeline_length"];
			if (!timeline_length.is_none()) {
				timeline_max_position_ = extract<float>(timeline_length);
			}
			object timeline_color_1 = configuration_["timeline_colors"][0];
			timeline_color_1_ = {
				extract<float>(timeline_color_1[0]),
				extract<float>(timeline_color_1[1]),
				extract<float>(timeline_color_1[2]),
				extract<float>(timeline_color_1[3])};
			object timeline_color_2 = configuration_["timeline_colors"][1];
			timeline_color_2_ = {
				extract<float>(timeline_color_2[0]),
				extract<float>(timeline_color_2[1]),
				extract<float>(timeline_color_2[2]),
				extract<float>(timeline_color_2[3])};

			CommandQueueLocation timeline_command;
			std::tie(timeline_graphics_, timeline_command) = scene_->AddActorReturnInitialize(make_unique<GraphicsObject>());
			game_scene::actors::GraphicsObjectDetails square_details;
			square_details.heirarchy_ = game_scene::actors::ComponentHeirarchy(
				"square",
				{
					{"square.obj|Square",
					ObjLoader::OutputFormat(
						ModelModifier(
							{0, 1, 2},
							{1, 1, 1},
							{false, true}),
						VertexType::vertex_type_all,
						false)}
				},
				{});
			square_details.heirarchy_.shader_file_definition_ = game_scene::actors::ShaderFileDefinition("timeline.hlsl");
			square_details.heirarchy_.vertex_shader_input_type_ = VertexType::vertex_type_all;
			square_details.heirarchy_.entity_group_ = "basic";
			square_details.heirarchy_.shader_settings_ = {
				timeline_color_1_,
				timeline_color_2_,
				{timeline_position_ / timeline_max_position_},
			};
			timeline_command = scene_->MakeCommandAfter(timeline_command, Command(
				Target(timeline_graphics_),
				make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
					game_scene::GraphicsObjectCommand::CREATE_COMPONENTS,
					square_details)));
			timeline_command = scene_->MakeCommandAfter(timeline_command, Command(
				Target(timeline_graphics_),
				make_unique<commands::ComponentPlacement>("square",
					Pose(Location(0.0f, 1.5f, -7.5f), Quaternion::RotationAboutAxis(AID_X, 3.1416f/2.0f), Scale(5.0f, 1.0f, 0.1f)).GetMatrix())));

			headset_interface_ = scene_->FindByName("HeadsetInterface");
			if (headset_interface_ != ActorId::UnsetId) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(headset_interface_),
					make_unique<commands::HeadsetListenerRegistration>(true, id_, HeadsetInterface::ListenerId::TRIGGER_STATE_CHANGE)));
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(headset_interface_),
					make_unique<commands::HeadsetListenerRegistration>(true, id_, HeadsetInterface::ListenerId::TOUCHPAD_SLIDE)));
			}

			CreateGraphicsResources();
		} catch (error_already_set) {
			PyErr_Print();
		}
	}
	break;
	case HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE:
	{
		const auto& trigger_state_change = dynamic_cast<const commands::TriggerStateChange&>(args);
		if (trigger_state_change.controller_number_ == 0) {
			if (trigger_state_change.is_pulled_) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(headset_interface_),
					make_unique<commands::HeadsetListenerRegistration>(true, id_, HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT)));
			} else {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(headset_interface_),
					make_unique<commands::HeadsetListenerRegistration>(false, id_, HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT)));
			}
		}
	}
	break;
	case HeadsetInterfaceCommand::LISTEN_TOUCHPAD_SLIDE:
	{
		const auto& touchpad_slide = dynamic_cast<const commands::TouchpadMotion&>(args);
		if (touchpad_slide.controller_number_ == 0) {
			MoveTimelinePosition(touchpad_slide.delta_.x / 2.0f);
		}
	}
	break;
	case HeadsetInterfaceCommand::LISTEN_CONTROLLER_MOVEMENT:
	{
		const commands::ControllerMovement& controller_movement = dynamic_cast<const commands::ControllerMovement&>(args);
		if (controller_movement.controller_number_ == 0) {
			for (const pair<string, ActorId>& vertex_actor : vertex_actors_) {
				scene_->MakeCommandAfter(
					scene_->FrontOfCommands(),
					Command(
						Target(vertex_actor.second),
						make_unique<WrappedCommandArgs<Location>>(
							NichijouCommand::MOVE_VERTEX_LOCATION,
							controller_movement.movement_vector_)));
			}
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
	case NichijouQuery::GET_VERTEX:
	{
		return make_unique<QueryResultWrapped<ActorId>>(NichijouQuery::GET_VERTEX, vertex_actors_[dynamic_cast<const WrappedQueryArgs<string>&>(args).data_]);
	}
	default:
		break;
	}
	return make_unique<EmptyQueryResult>();
}

void NichijouGraph::CreateGraphicsResources() {
	actors::GraphicsResources& resources =
		dynamic_cast<const QueryResultWrapped<actors::GraphicsResources&>&>(*scene_->AskQuery(
			Target(scene_->FindByName("GraphicsResources")),
			QueryArgs(GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST))).data_;
	ModelGenerator gen(VertexType::vertex_type_texture, D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
	gen.AddVertexBatch(Vertices(VertexType::vertex_type_texture,
	vector<array<float,3>>{
		array<float,3>{0.0f, 0.0f, 0.0f},
		array<float,3>{1.0f, 0.0f, 0.0f},
	}));
	gen.Finalize(resources.device_interface_, optional<EntityHandler&>{}, ModelStorageDescription::Immutable());
	gen.parts_ = { { "Line", ModelSlice(gen.GetCurrentNumberOfVertices(), 0) } };
	ResourceIdent edge_model_ident(ResourceIdent::MODEL, edge_model_name_, gen);
	scene_->MakeCommandAfter(
		scene_->FrontOfCommands(),
		Command(
			Target(scene_->FindByName("GraphicsResources")),
			make_unique<WrappedCommandArgs<ResourceIdent>>(
				GraphicsObjectCommand::REQUIRE_RESOURCE, edge_model_ident)));
}

void NichijouGraph::MoveTimelinePosition(float delta) {
	timeline_position_ = min(max(timeline_position_ + delta, 0.0f), timeline_max_position_);
	std::cout << timeline_position_ << std::endl;
	for (const auto& vertex : vertex_actors_) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(vertex.second), make_unique<commands::TimelinePosition>(timeline_position_, timeline_max_position_)));
	}
	for (const auto& edge : edge_actors_) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(edge), make_unique<commands::TimelinePosition>(timeline_position_, timeline_max_position_)));
	}
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
		Target(timeline_graphics_), make_unique<WrappedCommandArgs<tuple<string, ShaderSettingsValue>>>(
			GraphicsObjectCommand::SET_SHADER_VALUES, make_tuple("square", ShaderSettingsValue(vector<vector<float>>({
				timeline_color_1_,
				timeline_color_2_,
				{timeline_position_ / timeline_max_position_}}))))));
}

void NichijouVertex::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
		try {
			CommandQueueLocation sprite_command;
			std::tie(vertex_graphics_, sprite_command) = scene_->AddActorReturnInitialize(make_unique<Sprite>());
			sprite_command = scene_->MakeCommandAfter(sprite_command,
				Command(Target(vertex_graphics_), make_unique<commands::SpriteDetails>(
					string(extract<string>(configuration_["shader_name"])),
					string(extract<string>(vertex_.attr("texture_name"))))));
			sprite_command = scene_->MakeCommandAfter(sprite_command,
				Command(Target(vertex_graphics_), make_unique<commands::SpritePlacement>(Location(0, 0, 0), array<float, 2>{1.0f, 1.0f})));
		} catch (error_already_set) {
			PyErr_Print();
		}
	}
	break;
	case NichijouCommand::TELL_VERTEX_ABOUT_EDGE:
	{
		edges_.push_back(dynamic_cast<const WrappedCommandArgs<ActorId>&>(args).data_);
	}
	break;
	case NichijouCommand::SET_VERTEX_LOCATION:
	{
		location_ = dynamic_cast<const WrappedCommandArgs<Location>&>(args).data_;
		UpdateAllEdgeLocations();
		float radius = extract<float>(configuration_["radius"]);
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(vertex_graphics_),
			make_unique<commands::SpritePlacement>(
				location_, array<float, 2>{ radius, radius })));
	}
	break;
	case NichijouCommand::MOVE_VERTEX_LOCATION:
	{
		location_ += dynamic_cast<const WrappedCommandArgs<Location>&>(args).data_;
		UpdateAllEdgeLocations();
		float radius = extract<float>(configuration_["radius"]);
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(vertex_graphics_),
			make_unique<commands::SpritePlacement>(
				location_, array<float, 2>{ radius, radius })));
	}
	break;
	case NichijouCommand::SET_VISIBLE:
	{
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(vertex_graphics_),
			make_unique<commands::ComponentDrawnState>(
				"point", dynamic_cast<const WrappedCommandArgs<bool>&>(args).data_)));
	}
	break;
	case NichijouCommand::TELL_TIMELINE_POSITION:
		try {
			bool new_visible = extract<bool>(vertex_.attr("time_range").attr("InRange")(dynamic_cast<const commands::TimelinePosition&>(args).position_));
			if (new_visible != visible_) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(vertex_graphics_),
					make_unique<commands::ComponentDrawnState>(
						"point", new_visible)));
				visible_ = new_visible;
			}
		} catch (error_already_set) {
				PyErr_Print();
		}
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

void NichijouVertex::UpdateEdgeLocation(ActorId edge_actor) {
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(edge_actor),
		make_unique<WrappedCommandArgs<tuple<ActorId, Location>>>(NichijouCommand::SET_EDGE_LOCATION, tuple<ActorId, Location>(id_, location_))));
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
		u_actor_ = dynamic_cast<QueryResultWrapped<ActorId>&>(*scene_->AskQuery(Target(graph_), WrappedQueryArgs<string>(NichijouQuery::GET_VERTEX, string(extract<string>(edge_.attr("u_name")))))).data_;
		v_actor_ = dynamic_cast<QueryResultWrapped<ActorId>&>(*scene_->AskQuery(Target(graph_), WrappedQueryArgs<string>(NichijouQuery::GET_VERTEX, string(extract<string>(edge_.attr("v_name")))))).data_;
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(u_actor_), make_unique<WrappedCommandArgs<ActorId>>(NichijouCommand::TELL_VERTEX_ABOUT_EDGE, id_)));
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(v_actor_), make_unique<WrappedCommandArgs<ActorId>>(NichijouCommand::TELL_VERTEX_ABOUT_EDGE, id_)));

		actors::GraphicsObjectDetails line_details;
		line_details.heirarchy_ = actors::ComponentHeirarchy(
			"line",
			{
				{model_name_ + "|Line",
				ObjLoader::OutputFormat(
					ModelModifier(
						{0, 1, 2},
						{1, 1, 1},
						{false, true}),
					VertexType::vertex_type_location,
					false)}
			},
			{});
		line_details.heirarchy_.shader_file_definition_ = actors::ShaderFileDefinition("solidline.hlsl", {true, true, true});
		line_details.heirarchy_.vertex_shader_input_type_ = VertexType::vertex_type_location;
		line_details.heirarchy_.entity_group_ = "basic";
		line_details.heirarchy_.shader_settings_ = {
			{1.0f, 0.0f, 0.0f, 1.0f},
			{ 1.0f }
		};

		CommandQueueLocation last_command;
		std::tie(edge_graphics_, last_command) = scene_->AddActorReturnInitialize(make_unique<actors::GraphicsObject>());
		last_command = scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(edge_graphics_), 
			make_unique<WrappedCommandArgs<actors::GraphicsObjectDetails>>(
				GraphicsObjectCommand::CREATE_COMPONENTS,
				line_details)));
		last_command = scene_->MakeCommandAfter(last_command, Command(
			Target(edge_graphics_),
			make_unique<commands::ComponentPlacement>(
				"line", DirectX::XMMatrixIdentity())));
	}
	break;
	case NichijouCommand::SET_EDGE_LOCATION:
	{
		const tuple<ActorId, Location>& new_location = dynamic_cast<const WrappedCommandArgs<tuple<ActorId, Location>>&>(args).data_;
		if (get<0>(new_location) == u_actor_) {
			u_location_ = get<1>(new_location);
		} else if (get<0>(new_location) == v_actor_) {
			v_location_ = get<1>(new_location);
		}

		float line_width = extract<float>(configuration_["width"]);
		float vertex_spacing = extract<float>(configuration_["vertex_spacing"]);
		Location edge_vector = v_location_ - u_location_;
		float edge_vector_length = edge_vector.GetLength();
		Pose edge_pose(u_location_ + edge_vector * vertex_spacing / edge_vector_length,
			Quaternion::RotationBetweenVectors({1, 0, 0}, edge_vector.location_));
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(edge_graphics_),
			make_unique<commands::ComponentPlacement>(
				"line",
				DirectX::XMMatrixScaling(edge_vector.GetLength() - vertex_spacing * 2, 1, 1) * edge_pose.GetMatrix())));
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(edge_graphics_),
			make_unique<WrappedCommandArgs<tuple<string, ShaderSettingsValue>>>(
				GraphicsObjectCommand::SET_SHADER_VALUES,
				tuple<string, ShaderSettingsValue>(
					"line",
					ShaderSettingsValue(vector<vector<float>>({{1.0f, 0.0f, 0.0f, 1.0f}, {line_width}}))))));
	}
	break;
	case NichijouCommand::TELL_TIMELINE_POSITION:
		try {
			bool new_visible = extract<bool>(edge_.attr("time_range").attr("InRange")(dynamic_cast<const commands::TimelinePosition&>(args).position_));
			if (new_visible != visible_) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
					Target(edge_graphics_),
					make_unique<commands::ComponentDrawnState>(
						"line", new_visible)));
				visible_ = new_visible;
			}
		} catch (error_already_set) {
				PyErr_Print();
		}
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

}  // actors
}  // game_scene
