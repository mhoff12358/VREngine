#pragma once

#include "RenderGroup.h"
#include "AsyncEntityBuffer.h"
#include "Entity.h"
#include "EntityId.h"
#include "ResourcePool.h"

class EntityHandler
{
public:
	EntityHandler();
	~EntityHandler();

	void Initialize(ResourcePool* rp);
	void SetEntitySets(map<string, int>& entity_group_associations);

	// Rendering accessors
	RenderGroup* GetRenderGroupForDrawing();
	unsigned int GetNumEntitySets();

	// Update loop updaters
	void FinishUpdate();

private:
	unsigned int AddEntity(Entity new_entity, unsigned int entity_set_number);
public:
	unsigned int AddEntity(Entity new_entity, string entity_set_name);
	void DisableEntity(unsigned int external_entity_id);
	void EnableEntity(unsigned int external_entity_id);
	void RemoveEntity(unsigned int external_entity_id);

	void SetEntityVertexShader(unsigned int external_entity_id, VertexShader vertex_shader);
	void SetEntityPixelShader(unsigned int external_entity_id, PixelShader pixel_shader);
	
	template <typename T>
	void SetEntityObjectSettings(unsigned int external_entity_id, const T& new_settings);
	template <typename T>
	ConstantBufferTyped<T>* GetEntityObjectSettingsTyped(unsigned int external_entity_id);
	ConstantBuffer* GetEntityObjectSettings(unsigned int external_entity_id);

	template <typename T>
	ConstantBufferTyped<T>* GetShaderSettingsTyped(unsigned int external_entity_id);
	ConstantBuffer* GetShaderSettings(unsigned int external_entity_id);

	void AddModelMutation(std::string model_resource_id, ModelMutation new_model_mutation);

private:
	ResourcePool* resource_pool;

	// Stores a mapping of index in the entity_map to index in the RenderGroup
	std::vector<EntityId> entity_map;
	map<string, int> entity_group_associations_;
	unsigned int num_entity_sets_;

	AsyncEntityBuffer entity_buffers;
	RenderGroup* current_edit_group;

	Entity& ReferenceEntity(EntityId entity_id);
};

template <typename T>
ConstantBufferTyped<T>* EntityHandler::GetEntityObjectSettingsTyped(unsigned int entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	return dynamic_cast<ConstantBufferTyped<T>*>(ReferenceEntity(entity_offset).object_settings);
}

template <typename T>
void EntityHandler::SetEntityObjectSettings(unsigned int entity_id, const T& new_settings) {
	EntityId entity_offset = entity_map[external_entity_id];
	*dynamic_cast<T*>(ReferenceEntity(entity_offset).object_settings->EditBufferData()) = new_settings;
}

template <typename T>
ConstantBufferTyped<T>* EntityHandler::GetShaderSettingsTyped(unsigned int entity_id) {
	EntityId entity_offset = entity_map[external_entity_id];
	ConstantBuffer* buffer = ReferenceEntity(entity_offset).MutableShaderSettings().constant_buffer;
	return dynamic_cast<ConstantBufferTyped<T>*>(buffer);
}
