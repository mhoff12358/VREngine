#pragma once

#include "stdafx.h"

class BasePipelineStageDesc;
class PipelineStageDesc;
class RenderGroup;

enum PipelineStageType : unsigned char {
	PST_TEXTURE_COPY,
	PST_RENDER_ENTITIES,
	PST_PROCESSING_EFFECT,
};

class PipelineStage
{
public:
	PipelineStage(const BasePipelineStageDesc& description);
	virtual ~PipelineStage();

	virtual void Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw);
	PipelineStage* NextStage();
	static unsigned int StageSize(PipelineStageType type);

protected:
	string name_;
	PipelineStageType stage_type_;
};

class PipelineStageBuffer {
public:
	PipelineStageBuffer();

	void ApplyAll(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw);
	void PreallocateStage(const PipelineStageDesc& stage_desc);
	void AddStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const PipelineStageDesc& stage_desc);
	pair<void*, int> GetStage(int stage_number);

private:
	vector<unique_ptr<PipelineStage>> stages_;
	ID3D11ShaderResourceView* clear_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];
};

