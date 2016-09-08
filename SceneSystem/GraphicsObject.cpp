#include "stdafx.h"

#include "ConstantBufferDescribed.h"
#include "GraphicsObject.h"
#include "Scene.h"
#include "VRBackend/ResourcePool.h"

namespace game_scene {

REGISTER_COMMAND(GraphicsObjectCommand, REQUIRE_RESOURCE);
REGISTER_COMMAND(GraphicsObjectCommand, CREATE_COMPONENTS);
REGISTER_COMMAND(GraphicsObjectCommand, CREATE_COMPONENTS_ASSUME_RESOURCES);
REGISTER_COMMAND(GraphicsObjectCommand, PLACE_COMPONENT);
REGISTER_COMMAND(GraphicsObjectCommand, SET_SHADER_VALUES);
REGISTER_COMMAND(GraphicsObjectCommand, SET_COMPONENT_DRAWN);

namespace actors {

void ComponentHeirarchy::GetRequiredResources(vector<ResourceIdent>* resources) const {
	for (const tuple<string, ObjLoader::OutputFormat>& model_name : models_) {
		resources->push_back(ResourceIdent(ResourceIdent::MODEL, get<0>(model_name), get<1>(model_name)));
	}
	for (const TextureDetails& texture_details : textures_) {
		resources->push_back(ResourceIdent(
			ResourceIdent::TEXTURE,
			texture_details.name_));
	}
	if (shader_file_definition_.ShouldLoadStage(ShaderStage::VERTEX)) {
		resources->push_back(ResourceIdent(ResourceIdent::VERTEX_SHADER, shader_file_definition_.StageFileName(ShaderStage::VERTEX), vertex_shader_input_type_));
	}
	if (shader_file_definition_.ShouldLoadStage(ShaderStage::GEOMETRY)) {
		resources->push_back(ResourceIdent(ResourceIdent::GEOMETRY_SHADER, shader_file_definition_.StageFileName(ShaderStage::GEOMETRY)));
	}
	if (shader_file_definition_.ShouldLoadStage(ShaderStage::PIXEL)) {
		resources->push_back(ResourceIdent(ResourceIdent::PIXEL_SHADER, shader_file_definition_.StageFileName(ShaderStage::PIXEL)));
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
	case GraphicsObjectCommand::CREATE_COMPONENTS:
		RequestResourcesAndCreateComponents(
			dynamic_cast<const WrappedCommandArgs<GraphicsObjectDetails>&>(args).data_);
		break;
	case GraphicsObjectCommand::CREATE_COMPONENTS_ASSUME_RESOURCES:
		CreateComponents(
			dynamic_cast<const WrappedCommandArgs<GraphicsObjectDetails>&>(args).data_);
		break;
	case GraphicsObjectCommand::PLACE_COMPONENT:
		PlaceComponent(
			dynamic_cast<const commands::ComponentPlacement&>(args));
		break;
	case GraphicsObjectCommand::SET_SHADER_VALUES:
	{
		const auto& shader_value_data = dynamic_cast<const WrappedCommandArgs<tuple<string, ShaderSettingsValue>>&>(args).data_;
		SetShaderSettingsValue(get<0>(shader_value_data), get<1>(shader_value_data));
	}
		break;
	case GraphicsObjectCommand::SET_COMPONENT_DRAWN:
		SetComponentIsDrawn(dynamic_cast<const commands::ComponentDrawnState&>(args));
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
	for (const ResourceIdent resource : required_resources) {
		last_resource_load = scene_->MakeCommandAfter(
			last_resource_load,
			Command(
				Target(scene_->FindByName("GraphicsResources")),
				make_unique<WrappedCommandArgs<ResourceIdent>>(
					GraphicsObjectCommand::REQUIRE_RESOURCE, resource)));
	}
	// Create a new commands for this actor doing the actual component creation
	// which occurs after all the resources have been loaded.
	scene_->MakeCommandAfter(
		last_resource_load,
		Command(
			Target(id_),
			make_unique<WrappedCommandArgs<GraphicsObjectDetails>>(
				GraphicsObjectCommand::CREATE_COMPONENTS_ASSUME_RESOURCES,
				details)));
}

void GraphicsObject::CreateComponents(const GraphicsObjectDetails& details) {
	components_.reserve(details.heirarchy_.GetNumberOfComponents());

	actors::GraphicsResources& graphics_resources = GraphicsResources::GetGraphicsResources(scene_);

	components_.emplace_back(
		move(tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>(
			Component(graphics_resources.device_interface_),
			unique_ptr<ConstantBuffer>(nullptr),
			vector<unsigned int>({}))));
	CreateIndividualComponent(graphics_resources, details.heirarchy_, 0);
}

void GraphicsObject::CreateSettingsEntity(
	actors::GraphicsResources& graphics_resources,
	const ComponentHeirarchy& heirarchy, unsigned int reserved_space) {
	VertexShader vertex_shader;
	GeometryShader geometry_shader(false);
	PixelShader pixel_shader;
	if (heirarchy.shader_file_definition_.ShouldLoadStage(ShaderStage::VERTEX)) {
		vertex_shader = graphics_resources.resource_pool_.LoadExistingVertexShader(heirarchy.shader_file_definition_.StageFileName(ShaderStage::VERTEX));
	}
	if (heirarchy.shader_file_definition_.ShouldLoadStage(ShaderStage::GEOMETRY)) {
		geometry_shader = graphics_resources.resource_pool_.LoadExistingGeometryShader(heirarchy.shader_file_definition_.StageFileName(ShaderStage::GEOMETRY));
	}
	if (heirarchy.shader_file_definition_.ShouldLoadStage(ShaderStage::PIXEL)) {
		pixel_shader = graphics_resources.resource_pool_.LoadExistingPixelShader(heirarchy.shader_file_definition_.StageFileName(ShaderStage::PIXEL));
	}
	
	unique_ptr<ConstantBuffer>& shader_settings_buffer = get<1>(components_[reserved_space]);
	ShaderSettingsFormat settings_format = heirarchy.shader_settings_.GetFormat();
	if (settings_format.ShouldCreateBuffer()) {
		shader_settings_buffer = make_unique<ConstantBufferDescribed>(
			CB_PS_VERTEX_AND_GEOMETRY_AND_PIXEL_SHADER,
			settings_format);
		shader_settings_buffer->CreateBuffer(graphics_resources.device_interface_);
		heirarchy.shader_settings_.SetIntoConstantBuffer(shader_settings_buffer.get());
	}

	vector<unsigned int>& settings_entities = get<2>(components_[reserved_space]);
	if (heirarchy.textures_.size() == 0) {
		settings_entities.push_back(
			graphics_resources.entity_handler_.AddEntity(Entity(
				ES_SETTINGS,
				Shaders(vertex_shader, pixel_shader, geometry_shader),
				ShaderSettings(shader_settings_buffer.get()),
				Model(),
				NULL), heirarchy.entity_group_));
	} else {
		for (const TextureDetails& texture_details : heirarchy.textures_) {
			Texture texture = graphics_resources.resource_pool_.LoadTexture(texture_details.name_);
			TextureView texture_view = TextureView(
				0, 0,
				texture_details.use_in_vertex_,
				texture_details.use_in_pixel_,
				texture);
			settings_entities.push_back(
				graphics_resources.entity_handler_.AddEntity(Entity(
					ES_SETTINGS,
					Shaders(vertex_shader, pixel_shader, geometry_shader),
					ShaderSettings(shader_settings_buffer.get()),
					Model(),
					NULL,
					texture_view), heirarchy.entity_group_));
		}
	}
}

void GraphicsObject::CreateIndividualComponent(
	actors::GraphicsResources& graphics_resources,
	const ComponentHeirarchy& heirarchy, unsigned int reserved_space) {
	if (!heirarchy.children_.empty()) {
		// Recursively create all your children.
		unsigned int child_starting_index = components_.size();
		for (const ComponentHeirarchy& child : heirarchy.children_) {
			components_.emplace_back(
				move(tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>(
					Component(graphics_resources.device_interface_),
					unique_ptr<ConstantBuffer>(nullptr),
					vector<unsigned int>({}))));
		}
		for (int i = 0; i < heirarchy.children_.size(); i++) {
			CreateIndividualComponent(graphics_resources, heirarchy.children_[i], child_starting_index + i);
		}
		// Inform the children of their parent.
		get<0>(components_[reserved_space]).SetChildren(&get<0>(components_[child_starting_index]), heirarchy.children_.size());
	}

	component_lookup_[heirarchy.name_] = reserved_space;

	// Create own entities.
	CreateSettingsEntity(graphics_resources, heirarchy, reserved_space);
	vector<Model> models;
	for (const tuple<string, ObjLoader::OutputFormat>& model_name : heirarchy.models_) {
		models.push_back(graphics_resources.resource_pool_.LoadExistingModel(get<0>(model_name)));
	}
	get<0>(components_[reserved_space]).AddEntitiesToHandler(
		graphics_resources.entity_handler_,
		heirarchy.entity_group_,
		models);
}

tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>&
		GraphicsObject::LookupComponent(string component_name) {
	auto component_iter = component_lookup_.find(component_name);
	if (component_iter == component_lookup_.end()) {
		std::cout << "Attempting to look up a nonexistent component" << std::endl;
	}
	return components_[component_iter->second];
}

void GraphicsObject::PlaceComponent(const commands::ComponentPlacement& placement) {
	get<0>(LookupComponent(placement.component_name_)).
		SetLocalTransformation(placement.transformation_);
}
	
void GraphicsObject::SetShaderSettingsValue(string component_name, const ShaderSettingsValue& value) {
	value.SetIntoConstantBuffer(get<1>(LookupComponent(component_name)).get());
}

void GraphicsObject::SetComponentIsDrawn(const commands::ComponentDrawnState& component_drawn_state) {
	const auto& component = LookupComponent(component_drawn_state.component_name_);
	EntityHandler& entity_handler = GraphicsResources::GetGraphicsResources(scene_).entity_handler_;
	for (unsigned int entity_offset = 0; entity_offset < get<0>(component).number_of_entities_; entity_offset++) {
		if (component_drawn_state.is_drawn_) {
			entity_handler.EnableEntity(get<0>(component).first_entity_ + entity_offset);
		} else {
			entity_handler.DisableEntity(get<0>(component).first_entity_ + entity_offset);
		}
	}
	for (unsigned int settings_entity : get<2>(component)) {
		if (component_drawn_state.is_drawn_) {
			entity_handler.EnableEntity(settings_entity);
		} else {
			entity_handler.DisableEntity(settings_entity);
		}
	}
}

}  // actors
}  // game_scene