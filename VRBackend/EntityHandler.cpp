#include "EntityHandler.h"


EntityHandler::EntityHandler()
{
}


EntityHandler::~EntityHandler()
{
}

void EntityHandler::Initialize(ResourcePool* rp) {
	resource_pool = rp;
}

void EntityHandler::SetEntitySets(map<string, int>& entity_group_associations) {
	entity_group_associations_ = entity_group_associations;
	num_entity_sets_ = entity_group_associations_.size();
	current_edit_group = entity_buffers.Initialize(num_entity_sets_);
}

unsigned int EntityHandler::GetNumEntitySets() {
	return num_entity_sets_;
}

RenderGroup* EntityHandler::GetRenderGroupForDrawing() {
	return entity_buffers.ConsumerFinish();
}

void EntityHandler::FinishUpdate() {
	current_edit_group = entity_buffers.ProducerFinish();
}

unsigned int EntityHandler::AddEntity(Entity new_entity, unsigned int entity_set_number) {
	current_edit_group[entity_set_number].entities.push_back(new_entity);
	entity_map.push_back(EntityId(entity_set_number, current_edit_group[entity_set_number].entities.size() - 1));
	return entity_map.size() - 1;
}

unsigned int EntityHandler::AddEntity(Entity new_entity, string entity_set_name) {
	auto entity_set_id = entity_group_associations_.find(entity_set_name);
	if (entity_set_id == entity_group_associations_.end()) {
		std::cout << "Attempting to add an entity to set \"" << entity_set_name << "\" which doesn't exist" << std::endl;
		return 0;
	}
	return AddEntity(new_entity, entity_group_associations_[entity_set_name]);
}

void EntityHandler::EnableEntity(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	ReferenceEntity(entity_offset).status &= ~ES_DISABLED;
}

void EntityHandler::DisableEntity(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	ReferenceEntity(entity_offset).status |= ES_DISABLED;
}

void EntityHandler::RemoveEntity(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	ReferenceEntity(entity_offset).status =
		ES_DELETED | ReferenceEntity(entity_offset).status;
}

void EntityHandler::SetEntityVertexShader(unsigned int external_entity_id, VertexShader vertex_shader) {
	EntityId entity_offset = entity_map[external_entity_id];
	ReferenceEntity(entity_offset).SetVertexShader(vertex_shader);
}

void EntityHandler::SetEntityPixelShader(unsigned int external_entity_id, PixelShader pixel_shader) {
	EntityId entity_offset = entity_map[external_entity_id];
	ReferenceEntity(entity_offset).SetPixelShader(pixel_shader);
}

Entity& EntityHandler::ReferenceEntity(EntityId entity_id) {
	return current_edit_group[entity_id.render_group_index_].entities[entity_id.entity_index_];
}

ConstantBuffer* EntityHandler::GetEntityObjectSettings(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	return ReferenceEntity(entity_offset).object_settings;
}

ConstantBuffer* EntityHandler::GetShaderSettings(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	return ReferenceEntity(entity_offset).MutableShaderSettings().constant_buffer;
}

void EntityHandler::AddModelMutation(std::string model_resource_id, ModelMutation new_model_mutation) {
	current_edit_group->model_mutations.push_back(std::make_pair(model_resource_id, new_model_mutation));
}