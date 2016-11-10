#pragma once

#include "stdafx.h"

#include "Texture.h"
#include "PipelineStage.h"
#include "PipelineStageDesc.h"
#include "RenderMode.h"

class EntityHandler;
class InputHandler;
class Headset;
class RenderGroup;
class ResourcePool;
class ViewState;

class RenderingPipeline {
public:
	RenderingPipeline();

	virtual void Render(RenderGroup* group_to_draw) = 0;

	void InitializeBackBuffer();
	Texture& GetFirstStageBuffer();

	void CallPipeline(RenderGroup* groups_to_draw);

	void SetPipelineStages(ResourcePool& resource_pool, vector<unique_ptr<PipelineStageDesc>>& stage_descs);

	array<int, 2> GetStageBufferSize();
	D3D11_TEXTURE2D_DESC* GetStagingBufferDesc();

protected:
	void AssignReferences(ViewState* vs, InputHandler* ih, EntityHandler* eh, string rl);
	void InitializeResources(array<int, 2> backing_buffers_size, D3D11_TEXTURE2D_DESC* stage_texture_desc);

	array<int, 2> back_buffer_size;
	D3D11_TEXTURE2D_DESC back_buffer_texture_desc;

public: // testing
	PipelineStageBuffer pipeline_stages_;
	map<string, int> entity_group_associations_;
	vector<Texture> pipeline_textures_;
	vector<DepthTexture> pipeline_depth_textures_;

	unsigned int num_stages;
	vector<Texture> stage_buffers;
	vector<RenderMode> stage_render_modes;
	vector<ID3D11BlendState*> stage_blend_states;
	
	DepthTexture depth_buffer;
	ID3D11DepthStencilState* depth_buffer_state;
	ID3D11DepthStencilView* depth_buffer_view;
	ViewState* view_state;
	InputHandler* input_handler;
	EntityHandler* entity_handler;
	string resource_location;
	Texture* back_buffer;

private:
	void InitializeDepthBuffer();
	virtual vector<TextureIdent> GetExistingTextureIdents() const = 0;
};

class ToScreenRenderingPipeline : public RenderingPipeline {
public:
	ToScreenRenderingPipeline() {}

	void Initialize(ViewState* vs, InputHandler* ih, EntityHandler* eh, string rl);

	void Render(RenderGroup* group_to_draw) override;

private:
	vector<TextureIdent> GetExistingTextureIdents() const override;
};

class ToHeadsetRenderingPipeline : public RenderingPipeline {
public:
	ToHeadsetRenderingPipeline() {}

	void Initialize(ViewState* vs, InputHandler* ih, EntityHandler* eh, string rl, Headset* ocu);

	void Render(RenderGroup* group_to_draw) override;

public:
	D3D11_VIEWPORT eye_viewports[2];

	Headset* oculus;

	void FillPipelineTexturesFromHeadset();

	vector<TextureIdent> GetExistingTextureIdents() const override;
};