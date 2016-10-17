#include "RenderGroup.h"
#include "ResourcePool.h"

RenderGroup::RenderGroup()
{
}

RenderGroup::~RenderGroup()
{
}


void RenderGroup::AddBufferCopy(ID3D11Buffer* dest_buffer, ID3D11Buffer* src_buffer) {
	buffer_copies_.push_back(make_pair(dest_buffer, src_buffer));
}

void RenderGroup::Update(RenderGroup* other) {
	entities = other->entities;
	cameras = other->cameras;
}

void RenderGroup::ApplyMutations(ID3D11Device* device, ID3D11DeviceContext* device_context, ResourcePool& resource_pool) {
	for (const pair<string, ModelMutation>& mutation : model_mutations) {
		resource_pool.UpdateModel(mutation.first, mutation.second);
	}
	for (const pair<ID3D11Buffer*, ID3D11Buffer*>& copy_instruction : buffer_copies_) {
		device_context->CopyResource(copy_instruction.first, copy_instruction.second);
	}
}

void RenderGroup::Draw(ID3D11Device* device, ID3D11DeviceContext* device_context) {
	for (Entity& entity : entities) {
		entity.Draw(device, device_context);
	}
}

void RenderGroup::Cleanup() {
	model_mutations.clear();
	buffer_copies_.clear();
}