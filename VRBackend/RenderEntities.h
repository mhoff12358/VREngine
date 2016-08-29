#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>
#include "PipelineStageDesc.h"
#include "RenderMode.h"
#include "PipelineStageWithRenderMode.h"

class RenderEntities : public PipelineStageWithRenderMode
{
public:
	RenderEntities(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const RenderEntitiesDesc& description);
	~RenderEntities();

	void Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) override;

private:
	const DirectX::XMMATRIX& view_matrix_;
	const DirectX::XMMATRIX& view_projection_matrix_;
	const DirectX::XMMATRIX& orientation_projection_matrix_;
	ConstantBufferTyped<ViewProjectionMatrixData> camera_transformation_buffer_;
	int entity_set_index_;
};

