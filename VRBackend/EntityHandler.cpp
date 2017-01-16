#include "stdafx.h"
#include "EntityHandler.h"

#include "RenderGroup.h"
#include "ModelMutation.h"
#include "ResourcePool.h"

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

void EntityHandler::SetLighting(ID3D11Device* device_interface, const map<LightingSystemIdent, unsigned int>& light_map) {
	light_map_ = light_map;
	auto& lights = current_edit_group[0].lights;
	lights.resize(light_map_.size());
	for (auto& light : lights) {
		light_buffers_.emplace_back(ShaderStages::All(), light.GetConstantBufferSize());
		light_buffers_.back().CreateBuffer(device_interface);
		light.SetConstantBuffer(&light_buffers_.back());
	}
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

void EntityHandler::CycleGraphics() {
	entity_buffers.BlockUntilConsumerDone();
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
	return current_edit_group[0].cameras[GetCameraIndex(camera_name)];
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

LightSystem& EntityHandler::MutableLightSystem(LightingSystemIdent light_name) {
	return current_edit_group[0].lights[GetLightSystemIndex(light_name)];
}

unsigned int EntityHandler::GetLightSystemIndex(LightingSystemIdent light_name) {
	if (!light_map_.count(light_name)) {
		std::cout << "Attempting to look up nonexistant light" << std::endl;
		return 0;
	}
	return light_map_[light_name];
}

void EntityHandler::AddLightSystem(ID3D11Device* device_interface, LightingSystemIdent light_name, LightSystem light_system) {
	if (light_map_.count(light_name)) {
		std::cout << "Attempting to recreate an existing pipeline light" << std::endl;
	}
	auto& lights = current_edit_group[0].lights;
	lights.emplace_back(std::move(light_system));
	light_buffers_.emplace_back(ShaderStages::All(), lights.back().GetConstantBufferSize());
	light_buffers_.back().CreateBuffer(device_interface);
	lights.back().SetConstantBuffer(&light_buffers_.back());
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
	
void EntityHandler::ClearAllEntities() {
	for (int i = 0; i < num_entity_sets_; i++) {
		current_edit_group[i].ClearDrawingSettings();
	}
}
