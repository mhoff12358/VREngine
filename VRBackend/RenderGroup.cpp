#include "RenderGroup.h"


RenderGroup::RenderGroup()
{
}

RenderGroup::~RenderGroup()
{
}

void RenderGroup::Update(RenderGroup* other) {
	entities = other->entities;
}

void RenderGroup::ApplyMutations(ResourcePool& resource_pool) {
	for (const std::pair<std::string, ModelMutation>& mutation : model_mutations) {
		resource_pool.UpdateModel(mutation.first, mutation.second);
	}
}

void RenderGroup::Draw(ID3D11Device* device, ID3D11DeviceContext* device_context) {
	for (Entity& entity : entities) {
		entity.Draw(device, device_context);
	}
}

void RenderGroup::Cleanup() {
	model_mutations.clear();
}