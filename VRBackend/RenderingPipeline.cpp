#include "stdafx.h"
#include "RenderingPipeline.h"

#include "PipelineTexturePlanner.h"
#include "ViewState.h"
#include "Headset.h"
#include "EntityHandler.h"

RenderingPipeline::RenderingPipeline() {
}

void RenderingPipeline::AssignReferences(ViewState* vs, InputHandler* ih, EntityHandler* eh, std::string rl) {
	view_state = vs;
	input_handler = ih;
	resource_location = rl;
	entity_handler = eh;
}

Texture& RenderingPipeline::GetFirstStageBuffer() {
	return stage_buffers[0];
}

std::array<int, 2> RenderingPipeline::GetStageBufferSize() {
	return back_buffer_size;
}

D3D11_TEXTURE2D_DESC* RenderingPipeline::GetStagingBufferDesc() {
	return &back_buffer_texture_desc;
}

void RenderingPipeline::SetPipelineStages(ResourcePool& resource_pool, vector<unique_ptr<PipelineStageDesc>>& stage_descs) {
	PipelineTexturePlanner texture_planner(view_state->device_interface, view_state->device_context, pipeline_textures_, entity_group_associations_, resource_pool, *entity_handler);
	texture_planner.ParsePipelineStageDescs(GetExistingTextureIdents(), stage_descs);
	
	for (const unique_ptr<PipelineStageDesc>& stage_desc : stage_descs) {
		pipeline_stages_.AddStage(view_state->device_interface, view_state->device_context, *stage_desc);
	}
}

void RenderingPipeline::InitializeDepthBuffer() {
	depth_buffer.Initialize(view_state->device_interface, view_state->device_context, back_buffer_size);

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
	depth_stencil_desc.DepthEnable = true;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

	depth_stencil_desc.StencilEnable = true;
	depth_stencil_desc.StencilReadMask = 0xFF;
	depth_stencil_desc.StencilWriteMask = 0xFF;

	depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	view_state->device_interface->CreateDepthStencilState(&depth_stencil_desc, &depth_buffer_state);

	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	view_state->device_interface->CreateDepthStencilView(depth_buffer.GetTexture(), NULL, &depth_buffer_view);
}

void RenderingPipeline::CallPipeline(RenderGroup* groups_to_draw) {
	for (const Texture& output_texture : pipeline_textures_) {
		if (output_texture.GetIsRenderTarget()) {
			view_state->device_context->ClearRenderTargetView(output_texture.GetRenderTargetView(), D3DXCOLOR(0.0f, 1.0f, 0.0f, 0.0f));
		}
		if (output_texture.GetIsDepthStencil()) {
			view_state->device_context->ClearDepthStencilView(output_texture.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 1);
		}
	}

	pipeline_stages_.ApplyAll(view_state->device_interface, view_state->device_context, groups_to_draw);
}

void RenderingPipeline::InitializeBackBuffer() {
	pipeline_textures_.emplace_back(TextureUsage::RenderTarget());
	back_buffer = &pipeline_textures_.back();
	back_buffer->Initialize(view_state->device_interface, view_state->device_context, view_state->window_details.screen_size, view_state->back_buffer_texture);

	back_buffer_texture_desc = back_buffer->GetDesc();
	back_buffer_size[0] = back_buffer_texture_desc.Width;
	back_buffer_size[1] = back_buffer_texture_desc.Height;
}

void ToScreenRenderingPipeline::Initialize(ViewState* vs, InputHandler* ih, EntityHandler* eh, std::string rl) {
	RenderingPipeline::AssignReferences(vs, ih, eh, rl);

	InitializeBackBuffer();
}

void ToScreenRenderingPipeline::Render() {
	// Update the camera's position and transformations based on the world state
	RenderGroup* group_to_draw = entity_handler->GetRenderGroupForDrawing();
	
	CallPipeline(group_to_draw);
	view_state->swap_chain->Present(0, 0);
}

vector<TextureIdent> ToScreenRenderingPipeline::GetExistingTextureIdents() const {
	vector<TextureIdent> res;
	res.push_back("back_buffer");
	return res;
}

void ToHeadsetRenderingPipeline::Initialize(ViewState* vs, InputHandler* ih, EntityHandler* eh, std::string rl, Headset* ocu) {
	RenderingPipeline::AssignReferences(vs, ih, eh, rl);
	oculus = ocu;

	// Reserve the first 2 textures for the eye textures.
	pipeline_textures_.insert(pipeline_textures_.begin(), 2, Texture());
	// Fill the 3rd texture with the screen's texture.
	InitializeBackBuffer();
	// Fill the first 2 textures with the eye textures.
	oculus->CreateEyeTextures(vs->device_interface, vs->device_context, *GetStagingBufferDesc());
	FillPipelineTexturesFromHeadset();
}

void ToHeadsetRenderingPipeline::Render() {
	RenderGroup* group_to_draw = entity_handler->GetRenderGroupForDrawing();

	CallPipeline(group_to_draw);
	oculus->SubmitForRendering();
	view_state->swap_chain->Present(0, 0);
	oculus->UpdateRenderingPoses();
}

void ToHeadsetRenderingPipeline::FillPipelineTexturesFromHeadset() {
	for (const auto& eye : Headset::eyes_) {
		pipeline_textures_[eye] = oculus->GetEyeTexture(eye);
	}
}

vector<TextureIdent> ToHeadsetRenderingPipeline::GetExistingTextureIdents() const {
	vector<TextureIdent> res;
	res.push_back("back_buffer|0");
	res.push_back("back_buffer|1");
	res.push_back("back_buffer|2");
	return res;
}