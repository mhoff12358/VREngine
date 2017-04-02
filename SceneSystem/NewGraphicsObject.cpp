#include "stdafx.h"
#include "NewGraphicsObject.h"
#include "GraphicsResources.h"
#include "ConstantBufferDescribed.h"
#include "VRBackend/Entity.h"
#include "VRBackend/EntityHandler.h"

namespace game_scene {
namespace actors {

void NewGraphicsObjectImpl::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
		break;
	case NewGraphicsObjectCommand::CREATE:
		InitializeEntities(dynamic_cast<const commands::CreateNewGraphicsObject&>(args));
		break;
	case NewGraphicsObjectCommand::PLACE_COMPONENT:
		PlaceComponent(dynamic_cast<const commands::PlaceNewComponent&>(args));
		break;
	case NewGraphicsObjectCommand::SET_ENTITY_SHADER_VALUES:
		SetShaderValues(dynamic_cast<const commands::SetEntityShaderValues&>(args));
		break;
	default:
		FailToHandleCommand(args);
	}
}

void NewGraphicsObjectImpl::InitializeEntities(const commands::CreateNewGraphicsObject& args) {
	actors::GraphicsResources& graphics_resources = GraphicsResources::GetGraphicsResources(&GetScene());
	ResourcePool& resources = graphics_resources.resource_pool_;

	// Build the component heirarchy, discarding the builder when unnecessary.
	{
		ComponentHeirarchyBuilder heirarchy_builder(graphics_resources.device_interface_);
		heirarchy_builder.BuildComponents(args.components_, &transforms_, &transform_lookup_);
	}

	vector<Entity> entities;
	for (const EntitySpecification& entity_spec : args.entities_) {
		unsigned int first_entity_number = entities.size();
		// Emplaces the constant buffer for the shader settings (or a placeholding nullptr
		// if there should be no shader settings).
		if (entity_spec.shader_settings_format_.empty()) {
			constant_buffers_.emplace_back(nullptr);
		} else {
			if (entity_spec.shader_settings_value_.empty()) {
				constant_buffers_.emplace_back(move(make_unique<ConstantBufferDescribed>(
					entity_spec.shader_stages_,
					entity_spec.shader_settings_format_)));
			} else {
				constant_buffers_.emplace_back(move(make_unique<ConstantBufferDescribed>(
					entity_spec.shader_stages_,
					entity_spec.shader_settings_value_,
					graphics_resources.device_interface_)));
			}
		}

		// Finds the component transformation to use.
		ConstantBuffer* transform = nullptr;
 		Component* component = GetTransformByName(entity_spec.component_name_);
		if (component) {
			transform = component->GetTransformationBuffer();
		}

		vector<TextureView> texture_views = entity_spec.texture_details_.GetTextureViews(resources);
		if (texture_views.empty()) {
			texture_views.push_back(Entity::dummy_texture_view);
		}

		// If there's more than one texture they have to be added through other entities
		// which have to be created as settings entities before the main entity.
		for (size_t i = 0; i < texture_views.size() - 1; i++) {
			entities.emplace_back(texture_views[i]);
		}

		// Every entity specification requires at least one entity so create it and get a reference.
		entities.emplace_back(
			entity_spec.model_details_.IsActive() ? ES_NORMAL : ES_SETTINGS,
			entity_spec.shader_details_.GetShaders(resources),
			ShaderSettings(constant_buffers_.back().get()),
			entity_spec.model_details_.GetModel(resources),
			transform,
			texture_views.back()
		);
		entity_lookup_[entity_spec.entity_name_] = EntityRange(first_entity_number, entities.size() - first_entity_number);
	}
	EntityHandler& entity_handler = graphics_resources.entity_handler_;

	unsigned int num_entities = entities.size();
	entities_ = EntityRange(entity_handler.AddEntities(entities, args.entity_group_name_), num_entities);
}

void NewGraphicsObjectImpl::PlaceComponent(const commands::PlaceNewComponent& args) {
	Component* component = GetTransformByName(args.component_name_);
	if (component) {
		component->SetLocalTransformation(args.pose_.GetMatrix());
	}
}

void NewGraphicsObjectImpl::SetShaderValues(const commands::SetEntityShaderValues& args) {
	EntityRange all_entities = GetEntitiesByName(args.entity_name_);
	unsigned int shaded_entity = all_entities.GetMainEntity();

	actors::GraphicsResources& graphics_resources = GraphicsResources::GetGraphicsResources(&GetScene());
	EntityHandler& entity_handler = graphics_resources.entity_handler_;
	args.value_.SetIntoConstantBuffer(entity_handler.GetShaderSettings(shaded_entity));
}

Component* NewGraphicsObjectImpl::GetTransformByName(const string& transform_name) {
	Component* component = nullptr;
	auto transform_index = transform_lookup_.find(transform_name);
	if (transform_index == transform_lookup_.end()) {
		std::cout << "FAILED TO FIND COMPONENT BY NAME " << transform_name << std::endl;
	} else {
		component = &transforms_[transform_index->second];
	}
	return component;
}

NewGraphicsObjectImpl::EntityRange NewGraphicsObjectImpl::GetEntitiesByName(const string& entity_name) {
	auto entity_range = entity_lookup_.find(entity_name);
	if (entity_range == entity_lookup_.end()) {
		std::cout << "FAILED TO FIND ENTITY BY NAME " << entity_name << std::endl;
		return EntityRange(0, 0);
	} else {
		return entity_range->second.EvaluateInRange(entities_);
	}
}

}  // actors
}  /// game_scene