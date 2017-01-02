#pragma once

#include "stdafx.h"

#include "PipelineCamera.h"
#include "Lights.h"
#include "ModelMutation.h"
#include "Entity.h"

class ResourcePool;

class RenderGroup
{
public:
	RenderGroup();
	~RenderGroup();

	void ApplyMutations(ID3D11Device* device, ID3D11DeviceContext* device_context, ResourcePool& resource_pool);
	void Draw(ID3D11Device* device, ID3D11DeviceContext* device_context);
	void Update(RenderGroup* other);

	void Cleanup();

	void AddBufferCopy(ID3D11Buffer* dest_buffer, ID3D11Buffer* src_buffer);

	vector<Entity> entities;
	vector<PipelineCamera> cameras;
	vector<LightSystem> lights;
	vector<pair<string, ModelMutation>> model_mutations;
	vector<pair<ID3D11Buffer*, ID3D11Buffer*>> buffer_copies_;
};

