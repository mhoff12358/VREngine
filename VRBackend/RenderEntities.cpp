#include "stdafx.h"
#include "RenderEntities.h"

#include "PipelineStageDesc.h"
#include "PipelineCamera.h"
#include "Entity.h"
#include "RenderGroup.h"

RenderEntities::RenderEntities(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const RenderEntitiesDesc& description)
	: PipelineStageWithRenderMode(description),
	camera_transformation_buffer_(ShaderStages::All())
{
	assert(description.type_ == PST_RENDER_ENTITIES);

	InitializeRenderMode(dev, dev_con, description.output_textures_, true, description.blend_desc_, description.depth_stencil_view_, description.depth_stencil_state_);

	entity_set_index_ = description.entity_handler_set_;
	camera_index_ = description.camera_index_;
	light_index_ = description.light_index_;

	camera_transformation_buffer_.CreateBuffer(dev);
}


RenderEntities::~RenderEntities()
{
}

void RenderEntities::Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	PreApply();
	render_mode_.Prepare();
	
	const PipelineCamera& camera = groups_to_draw[0].cameras[camera_index_];
	camera_transformation_buffer_.SetAllTransformations(
		camera.GetViewMatrix(), camera.GetViewInverseMatrix(), camera.GetViewProjectionMatrix(), camera.GetOrientationProjectionMatrix());
	camera_transformation_buffer_.PushBuffer(dev_con);
	camera_transformation_buffer_.Prepare(dev, dev_con, PER_FRAME_CONSTANT_BUFFER_REGISTER);

	groups_to_draw[0].lights[light_index_].Prepare(dev, dev_con);

	vector<Entity>& entities = groups_to_draw[entity_set_index_].entities;
	for (Entity& entity : entities) {
		entity.Draw(dev, dev_con);
	}
}