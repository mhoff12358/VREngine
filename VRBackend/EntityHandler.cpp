#include "stdafx.h"
#include "EntityHandler.h"

#include "RenderGroup.h"
#include "ModelMutation.h"

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

void EntityHandler::SetCameras(const map<PipelineCameraIdent, unsigned int>& camera_map) {
	camera_map_ = camera_map;
	current_edit_group[0].cameras.resize(camera_map_.size());
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

unsigned int EntityHandler::AddEntities(const vector<Entity>& new_entities, unsigned int entity_set_number) {
	vector<Entity>& entity_set = current_edit_group[entity_set_number].entities;
	unsigned int first_new_set_id = entity_set.size();
	entity_set.insert(entity_set.end(), new_entities.cbegin(), new_entities.cend());
	unsigned int first_new_external_id = entity_map.size();
	entity_map.reserve(first_new_external_id + new_entities.size());
	for (size_t i = 0; i < new_entities.size(); i++) {
		entity_map.push_back(EntityId(entity_set_number, first_new_set_id + i));
	}
	return first_new_external_id;
}

unsigned int EntityHandler::AddEntity(Entity new_entity, string entity_set_name) {
	auto entity_set_id = entity_group_associations_.find(entity_set_name);
	if (entity_set_id == entity_group_associations_.end()) {
		std::cout << "Attempting to add an entity to set \"" << entity_set_name << "\" which doesn't exist" << std::endl;
		return 0;
	}
	return AddEntity(new_entity, entity_group_associations_[entity_set_name]);
}

unsigned int EntityHandler::AddEntities(const vector<Entity>& new_entities, string entity_set_name) {
	auto entity_set_id = entity_group_associations_.find(entity_set_name);
	if (entity_set_id == entity_group_associations_.end()) {
		std::cout << "Attempting to add an entity to set \"" << entity_set_name << "\" which doesn't exist" << std::endl;
		return 0;
	}
	return AddEntities(new_entities, entity_group_associations_[entity_set_name]);
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

PipelineCamera& EntityHandler::MutableCamera(PipelineCameraIdent camera_name) {
	return current_edit_group[0].cameras[camera_map_[camera_name]];
}

unsigned int EntityHandler::GetCameraIndex(PipelineCameraIdent camera_name) {
	if (!camera_map_.count(camera_name)) {
		std::cout << "Attempting to look up nonexistant camera" << std::endl;
		return 0;
	}
	return camera_map_[camera_name];
}

PipelineCamera& EntityHandler::AddCamera(PipelineCameraIdent camera_name) {
	if (camera_map_.count(camera_name)) {
		std::cout << "Attempting to recreate an existing pipeline camera" << std::endl;
		return MutableCamera(camera_name);
	}
	auto& cameras = current_edit_group[0].cameras;
	cameras.emplace_back();
	camera_map_[camera_name] = cameras.size() - 1;
	return cameras[camera_map_[camera_name]];
}

void EntityHandler::BuildCameras() {
	for (auto& camera : current_edit_group[0].cameras) {
		camera.BuildMatricesIfDirty();
	}
}

ConstantBuffer* EntityHandler::GetEntityObjectSettings(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	return ReferenceEntity(entity_offset).object_settings;
}

ConstantBuffer* EntityHandler::GetShaderSettings(unsigned int external_entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	return ReferenceEntity(entity_offset).MutableShaderSettings().constant_buffer;
}

void EntityHandler::AddModelMutation(std::string model_resource_id, ModelMutation&& new_model_mutation) {
	current_edit_group->model_mutations.emplace_back(std::make_pair(model_resource_id, new_model_mutation));
}

void EntityHandler::AddBufferCopy(ID3D11Buffer* dest_buffer, ID3D11Buffer* src_buffer) {
	current_edit_group[0].AddBufferCopy(dest_buffer, src_buffer);
}