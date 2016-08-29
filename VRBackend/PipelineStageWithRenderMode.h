#pragma once
#include "PipelineStage.h"
#include "RenderMode.h"

class PipelineStageWithRenderMode : public PipelineStage
{
public:
	PipelineStageWithRenderMode(const BasePipelineStageDesc& description);
	~PipelineStageWithRenderMode();

protected:
	void InitializeRenderMode(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const vector<Texture*>& output_textures, bool reload_textures_on_apply, D3D11_BLEND_DESC blend_desc, ID3D11DepthStencilView* depth_stencil_view, ID3D11DepthStencilState* depth_stencil_state);
	void PreApply();
	void SetRenderModeRenderTargetViews();

	RenderMode render_mode_;

	ID3D11BlendState* blend_state_;
	unsigned int num_output_textures_;
	Texture* output_textures_[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	bool reload_textures_on_apply_;
};

