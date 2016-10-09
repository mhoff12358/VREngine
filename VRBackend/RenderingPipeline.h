#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#include <string>
#include "stl.h"

#include "ViewState.h"
#include "RenderMode.h"
#include "InputHandler.h"
#include "PerspectiveCamera.h"
#include "Headset.h"
#include "EntityHandler.h"
#include "Entity.h"
#include "ProcessingEffect.h"
#include "PipelineStageDesc.h"
#include "PipelineStage.h"
#include "PipelineTexturePlanner.h"
#include "Quaternion.h"
#include "PipelineCamera.h"

class RenderingPipeline {
public:
	RenderingPipeline();

	virtual void Render() = 0;

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

	void Render() override;

private:
	vector<TextureIdent> GetExistingTextureIdents() const override;
};

class ToHeadsetRenderingPipeline : public RenderingPipeline {
public:
	ToHeadsetRenderingPipeline() {}

	void Initialize(ViewState* vs, InputHandler* ih, EntityHandler* eh, string rl, Headset* ocu);

	void Render() override;

public:
	D3D11_VIEWPORT eye_viewports[2];

	Headset* oculus;

	void FillPipelineTexturesFromHeadset();

	vector<TextureIdent> GetExistingTextureIdents() const override;
};