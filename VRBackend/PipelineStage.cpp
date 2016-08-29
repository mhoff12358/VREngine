#include "PipelineStage.h"
#include "RenderEntities.h"
#include "TextureCopy.h"
#include "ProcessingEffect.h"
#include "PipelineStageDesc.h"

PipelineStageBuffer::PipelineStageBuffer() {
	for (int i = 0; i < D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT; i++) {
		clear_shader_resource_views[i] = NULL;
	}
}

void PipelineStageBuffer::ApplyAll(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	PipelineStage* current_stage = reinterpret_cast<PipelineStage*>(stages_.data());
	PipelineStage* end_stage = reinterpret_cast<PipelineStage*>(stages_.data() + stages_.size());

	dev_con->VSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, clear_shader_resource_views);
	dev_con->PSSetShaderResources(0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT, clear_shader_resource_views);
	RenderMode::UnPrepare(dev_con);

	/*while (current_stage < end_stage) {
		current_stage->Apply(dev, dev_con, groups_to_draw);
		current_stage = current_stage->NextStage();
	}
	assert(current_stage == end_stage);*/
	for (const unique_ptr<PipelineStage>& stage : stages_) {
		stage->Apply(dev, dev_con, groups_to_draw);
	}
}

void PipelineStageBuffer::PreallocateStage(const PipelineStageDesc& stage_desc) {
	int original_capacity = stages_.capacity();
	stages_.reserve(original_capacity + stage_desc.GetSizeOfStage());
}

void PipelineStageBuffer::AddStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con, const PipelineStageDesc& stage_desc) {
	//int original_size = stages_.size();
	//stages_.resize(original_size + stage_desc.GetSizeOfStage());
	//stage_desc.ConstructStageInPlace(dev, dev_con, stages_.data() + original_size);
	for (PipelineStage* sub_stage : stage_desc.AllocateStage(dev, dev_con)) {
		stages_.emplace_back(sub_stage);
	}
}

pair<void*, int> PipelineStageBuffer::GetStage(int stage_number) {
	PipelineStage* current_stage = reinterpret_cast<PipelineStage*>(stages_.data());
	
	for (int i = 0; i < stage_number; i++) {
		current_stage = current_stage->NextStage();
	}
	return std::make_pair((void*)current_stage, ((char*)current_stage) - ((char*)stages_.data()));
}

PipelineStage::PipelineStage(const BasePipelineStageDesc& description)
{
	stage_type_ = description.type_;
	name_ = description.name_;
}


PipelineStage::~PipelineStage()
{
}

void PipelineStage::Apply(ID3D11Device* dev, ID3D11DeviceContext* dev_con, RenderGroup* groups_to_draw) {
	switch (stage_type_) {
	case PST_TEXTURE_COPY:
		reinterpret_cast<TextureCopy*>(this)->Apply(dev, dev_con, groups_to_draw);
		break;
	case PST_PROCESSING_EFFECT:
		reinterpret_cast<ProcessingEffect*>(this)->Apply(dev, dev_con, groups_to_draw);
		break;
	case PST_RENDER_ENTITIES:
		reinterpret_cast<RenderEntities*>(this)->Apply(dev, dev_con, groups_to_draw);
		break;
	}
}

PipelineStage* PipelineStage::NextStage() {
	return reinterpret_cast<PipelineStage*>(reinterpret_cast<char *>(this) + StageSize(stage_type_));
}

unsigned int PipelineStage::StageSize(PipelineStageType type) {
	switch (type) {
	case PST_TEXTURE_COPY:
		return sizeof(TextureCopy);
	case PST_PROCESSING_EFFECT:
		return sizeof(ProcessingEffect);
	case PST_RENDER_ENTITIES:
		return sizeof(RenderEntities);
	default:
		return sizeof(PipelineStage);
	}
}

/*
void PipelineStage::ConstructInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location, PipelineStageDesc description) {
	switch (description.type_) {
	case PST_TEXTURE_COPY:
		new (location)TextureCopy(dev, dev_con, description);
		break;
	case PST_PROCESSING_EFFECT:
		new (location)ProcessingEffect(dev, dev_con, description);
		break;
	case PST_RENDER_ENTITIES:
		new (location)RenderEntities(dev, dev_con, description);
		break;
	default:
		break;
	}
}
*/