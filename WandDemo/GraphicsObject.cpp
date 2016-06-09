#include "stdafx.h"

#include "ConstantBufferDescribed.h"
#include "GraphicsObject.h"
#include "Scene.h"
#include "ResourcePool.h"
#include "SpecializedCommandArgs.h"
#include "SpecializedActors.h"
#include "SpecializedQueries.h"

namespace game_scene {
namespace actors {

void ComponentHeirarchy::GetRequiredResources(vector<ResourceIdent>* resources) const {
	for (const tuple<string, ObjLoader::OutputFormat>& model_name : models_) {
		resources->push_back(ResourceIdent(ResourceIdent::MODEL, get<0>(model_name), get<1>(model_name)));
	}
}

int ComponentHeirarchy::GetNumberOfComponents() const {
	int number_of_subcomponents = 0;
	for (const ComponentHeirarchy& sub_heirarchies : children_) {
		number_of_subcomponents += sub_heirarchies.GetNumberOfComponents();
	}
	return 1 + number_of_subcomponents;
}

void GraphicsObject::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case commands::GraphicsCommandType::CREATE_COMPONENTS:
		RequestResourcesAndCreateComponents(
			dynamic_cast<const CommandArgsWrapper<GraphicsObjectDetails>&>(args).data_);
		break;
	case commands::GraphicsCommandType::CREATE_COMPONENTS_ASSUME_RESOURCES:
		CreateComponents(
			dynamic_cast<const CommandArgsWrapper<GraphicsObjectDetails>&>(args).data_);
		break;
	case commands::GraphicsCommandType::PLACE_COMPONENT:
		PlaceComponent(
			dynamic_cast<const commands::ComponentPlacement&>(args));
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

void GraphicsObject::RequestResourcesAndCreateComponents(const GraphicsObjectDetails& details) {
	// Create commands to load all the required resources.
	CommandQueueLocation last_resource_load = scene_->FrontOfCommands();
	vector<ResourceIdent> required_resources;
	details.heirarchy_.GetRequiredResources(&required_resources);
	for (const TextureDetails& texture_details : details.textures_) {
		required_resources.push_back(ResourceIdent(
			ResourceIdent::TEXTURE,
			texture_details.name_));
	}
	required_resources.push_back(ResourceIdent(ResourceIdent::PIXEL_SHADER, details.shader_name_, ObjLoader::vertex_type_all));
	for (const ResourceIdent resource : required_resources) {
		last_resource_load = scene_->MakeCommandAfter(
			last_resource_load,
			Command(
				Target(scene_->FindByName("GraphicsResources")),
				make_unique<CommandArgsWrapper<ResourceIdent>>(
					commands::GraphicsCommandType::REQUIRE_RESOURCE, resource)));
	}
	// Create a new commands for this actor doing the actual component creation
	// which occurs after all the resources have been loaded.
	scene_->MakeCommandAfter(
		last_resource_load,
		Command(
			Target(id_),
			make_unique<CommandArgsWrapper<GraphicsObjectDetails>>(
				commands::GraphicsCommandType::CREATE_COMPONENTS_ASSUME_RESOURCES,
				details)));
}

void GraphicsObject::CreateComponents(const GraphicsObjectDetails& details) {
	components_.reserve(details.heirarchy_.GetNumberOfComponents());

	Target graphics_resources_target = Target(scene_->FindByName("GraphicsResources"));

	unique_ptr<QueryResult> graphics_resources_result = scene_->AskQuery(
		graphics_resources_target,
		make_unique<QueryArgs>(
			queries::GraphicsResourceQueryType::GRAPHICS_RESOURCE_REQUEST));
	if (graphics_resources_result->Type() != queries::GraphicsResourceQueryType::GRAPHICS_RESOURCE_REQUEST) {
		std::cout << "UNEXPECTED RESPONSE WHILE GETTING GRAPHICS RESOURCES" << std::endl;
		return;
	}
	actors::GraphicsResources& graphics_resources = 
		dynamic_cast<QueryResultWrapped<actors::GraphicsResources&>*>(
			graphics_resources_result.get())->data_;

	CreateSettingsEntity(graphics_resources, details);

	components_.emplace_back(graphics_resources.device_interface_);
	CreateIndividualComponent(graphics_resources, details, details.heirarchy_, 0);
}

void GraphicsObject::CreateSettingsEntity(
	actors::GraphicsResources& graphics_resources,
	const GraphicsObjectDetails& details) {
	PixelShader pixel_shader = graphics_resources.resource_pool_.LoadExistingPixelShader(details.shader_name_);
	VertexShader vertex_shader = graphics_resources.resource_pool_.LoadExistingVertexShader(details.shader_name_);
	
	shader_settings_buffer_ = make_unique<ConstantBufferDescribed>(
		CB_PS_VERTEX_AND_PIXEL_SHADER,
		details.shader_settings_.GetFormat());
	shader_settings_buffer_->CreateBuffer(graphics_resources.device_interface_);
	float* raw_buffer = static_cast<float*>(shader_settings_buffer_->EditBufferData(true));
	for (const vector<float>& next_data_chunk : details.shader_settings_.GetValue()) {
		memcpy(raw_buffer, next_data_chunk.data(), sizeof(float) * next_data_chunk.size());
		raw_buffer = raw_buffer + next_data_chunk.size();
	}

	if (details.textures_.size() == 0) {
		settings_entities_.push_back(
			graphics_resources.entity_handler_.AddEntity(Entity(
				ES_SETTINGS,
				graphics_resources.resource_pool_.LoadExistingPixelShader(details.shader_name_),
				graphics_resources.resource_pool_.LoadExistingVertexShader(details.shader_name_),
				ShaderSettings(shader_settings_buffer_.get()),
				Model(),
				NULL), details.entity_group_));
	} else {
		for (const TextureDetails& texture_details : details.textures_) {
			Texture texture = graphics_resources.resource_pool_.LoadTexture(texture_details.name_);
			TextureView texture_view = TextureView(
				0, 0,
				texture_details.use_in_vertex_,
				texture_details.use_in_pixel_,
				texture);
			settings_entities_.push_back(
				graphics_resources.entity_handler_.AddEntity(Entity(
					ES_SETTINGS,
					graphics_resources.resource_pool_.LoadExistingPixelShader(details.shader_name_),
					graphics_resources.resource_pool_.LoadExistingVertexShader(details.shader_name_),
					ShaderSettings(shader_settings_buffer_.get()),
					Model(),
					NULL), details.entity_group_));
		}
	}
}

void GraphicsObject::CreateIndividualComponent(
	actors::GraphicsResources& graphics_resources, const GraphicsObjectDetails& details,
	const ComponentHeirarchy& component_heirarchy, unsigned int reserved_space) {
	if (!component_heirarchy.children_.empty()) {
		// Recursively create all your children.
		unsigned int child_starting_index = components_.size();
		for (const ComponentHeirarchy& child : component_heirarchy.children_) {
			components_.emplace_back(graphics_resources.device_interface_);
		}
		for (int i = 0; i < component_heirarchy.children_.size(); i++) {
			CreateIndividualComponent(graphics_resources, details, component_heirarchy.children_[i], child_starting_index + i);
		}
		// Inform the children of their parent.
		components_[reserved_space].SetChildren(&components_[child_starting_index], component_heirarchy.children_.size());
	}

	component_lookup_[component_heirarchy.name_] = reserved_space;

	// Create own entities.
	vector<Model> models;
	for (const tuple<string, ObjLoader::OutputFormat>& model_name : component_heirarchy.models_) {
		models.push_back(graphics_resources.resource_pool_.LoadExistingModel(get<0>(model_name)));
	}
	components_[reserved_space].AddEntitiesToHandler(
		graphics_resources.entity_handler_,
		details.entity_group_,
		models);
}

void GraphicsObject::PlaceComponent(const commands::ComponentPlacement& placement) {
	components_[component_lookup_[placement.component_name_]].
		SetLocalTransformation(placement.transformation_);
}

}  // actors
}  // game_scene