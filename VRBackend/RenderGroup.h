#pragma once

#include "Entity.h"
#include "ResourcePool.h"

class RenderGroup
{
public:
	RenderGroup();
	~RenderGroup();

	void ApplyMutations(ResourcePool& resource_pool);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* device_context);
	void Update(RenderGroup* other);

	void ExpandTo(unsigned int required_size);

	void Cleanup();

	std::vector<Entity> entities;
	std::vector<std::pair<std::string, ModelMutation>> model_mutations;
};

