#include "PipelineStageWithRenderMode.h"


PipelineStageWithRenderMode::PipelineStageWithRenderMode(const BasePipelineStageDesc& description)
	: PipelineStage(description)
{
}


PipelineStageWithRenderMode::~PipelineStageWithRenderMode()
{
}

void PipelineStageWithRenderMode::InitializeRenderMode(
	ID3D11Device* dev, ID3D11DeviceContext* dev_con,
	const vector<Texture*>& output_textures,
	bool reload_textures_on_apply,
	D3D11_BLEND_DESC blend_desc,
	ID3D11DepthStencilView* depth_stencil_view,
	ID3D11DepthStencilState* depth_stencil_state) {
	reload_textures_on_apply_ = reload_textures_on_apply;
	dev->CreateBlendState(&blend_desc, &blend_state_);

	num_output_textures_ = min(output_textures.size(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for (int i = 0; i < num_output_textures_; i++) {
		output_textures_[i] = output_textures[i];
	}

	render_mode_.Initialize(dev, dev_con, { {} }, depth_stencil_view, depth_stencil_state, blend_state_);
	if (num_output_textures_ != 0) {
		render_mode_.SetViewport(output_textures_[0]->GetSize(), std::array<float, 2>({ 0.0f, 1.0f }));
	}

	SetRenderModeRenderTargetViews();
}

void PipelineStageWithRenderMode::SetRenderModeRenderTargetViews() {
	ID3D11RenderTargetView** rtvs = render_mode_.MutateRenderTargetViews(num_output_textures_);
	for (int i = 0; i < num_output_textures_; i++) {
		rtvs[i] = output_textures_[i]->GetRenderTargetView();
	}
}

void PipelineStageWithRenderMode::PreApply() {
	if (reload_textures_on_apply_) {
		SetRenderModeRenderTargetViews();
	}
}