#pragma once

#include "stdafx.h"

#include "Entity.h"
#include "PipelineStageWithRenderMode.h"
#include "Model.h"
#include "Vertices.h"

class ConstantBuffer;
class TextureView;
class ResourcePool;
class RenderGroup;
class ProcessingEffectDesc;

class ProcessingEffect : public PipelineStageWithRenderMode
{
public:
	ProcessingEffect(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const ProcessingEffectDesc& description);
	~ProcessingEffect();

	static void CreateProcessingEffectResources(ResourcePool* resource_pool);

	void Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) override;

private:
public: //test
	Entity screen_filling_entity_;
	ConstantBuffer* settings_;
	unsigned int num_input_textures_;
	TextureView* input_textures_;

public:
	static VertexType squares_vertex_type;
	static Model CreateScreenFillingModel(ResourcePool* resource_pool);
	static Entity default_screen_filling_entity;
};
