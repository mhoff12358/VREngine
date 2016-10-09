#include "RenderEntities.h"

RenderEntities::RenderEntities(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const RenderEntitiesDesc& description)
	: PipelineStageWithRenderMode(description),
	view_projection_matrix_(description.GetViewProjectionMatrix()),
	view_matrix_(description.GetViewMatrix()),
	orientation_projection_matrix_(description.GetOrientationProjectionMatrix()),
	camera_transformation_buffer_(ShaderStages(ShaderStages::VERTEX_STAGE | ShaderStages::PIXEL_STAGE))
{
	assert(description.type_ == PST_RENDER_ENTITIES);
	/*
	dev->CreateBlendState(&description.blend_desc_, &blend_state_);

	vector<ID3D11RenderTargetView*> rtvs;
	for (Texture* texture : description.output_textures_) {
		rtvs.push_back(texture->GetRenderTargetView());
	}
	render_mode_.Initialize(dev, dev_con, rtvs, description.depth_stencil_view_, description.depth_stencil_state_, blend_state_);
	if (!rtvs.empty()) {
		render_mode_.SetViewport(description.output_textures_.front()->GetSize(), std::array<float, 2>({ 0.0f, 1.0f }));
	}*/

	InitializeRenderMode(dev, dev_con, description.output_textures_, true, description.blend_desc_, description.depth_stencil_view_, description.depth_stencil_state_);

	entity_set_index_ = description.entity_handler_set_;

	camera_transformation_buffer_.CreateBuffer(dev);
}


RenderEntities::~RenderEntities()
{
}

void RenderEntities::Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	PreApply();
	render_mode_.Prepare();
	
	camera_transformation_buffer_.SetAllTransformations(view_projection_matrix_, view_projection_matrix_, orientation_projection_matrix_);
	camera_transformation_buffer_.PushBuffer(dev_con);
	camera_transformation_buffer_.Prepare(dev, dev_con, PER_FRAME_CONSTANT_BUFFER_REGISTER);

	vector<Entity>& entities = groups_to_draw[entity_set_index_].entities;
	for (Entity& entity : entities) {
		entity.Draw(dev, dev_con);
	}
}