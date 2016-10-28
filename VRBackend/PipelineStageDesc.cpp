#include "stdafx.h"
#include "PipelineStageDesc.h"

#include "PipelineStage.h"
#include "Texture.h"
#include "PipelineCamera.h"
#include "TextureCopy.h"
#include "RenderEntities.h"
#include "ProcessingEffect.h"
#include "PipelineTexturePlanner.h"
#include "Shaders.h"

PipelineStageDesc::PipelineStageDesc() {

}

string PipelineStageDesc::GetBaseName(const string name) {
	size_t delimiter_found = name.find('|');
	if (delimiter_found != std::string::npos) {
		return name.substr(0, delimiter_found);
	}
	return name;
}

D3D11_BLEND_DESC BasePipelineStageDesc::DefaultBlendState() {
	D3D11_BLEND_DESC blend_state_desc;

	blend_state_desc.AlphaToCoverageEnable = false;
	blend_state_desc.IndependentBlendEnable = false;
	blend_state_desc.RenderTarget[0].BlendEnable = true;
	blend_state_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blend_state_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_state_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_state_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_state_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_state_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	return blend_state_desc;
}

BasePipelineStageDesc::BasePipelineStageDesc(
	string name,
	PipelineStageType type,
	vector<tuple<string, TextureSignature>> output_textures_descs,
	DepthStencilDescription depth_desc,
	vector<TextureIdent> input_textures_idents,
	D3D11_BLEND_DESC blend_desc)
	: PipelineStageDesc(),
	name_(name), type_(type), blend_desc_(blend_desc),
	output_textures_descs_(output_textures_descs),
	depth_desc_(depth_desc),
	input_textures_idents_(input_textures_idents)
{
	entity_handler_set_ = -1;
	depth_stencil_view_ = NULL;
	depth_stencil_state_ = NULL;
}

BasePipelineStageDesc::~BasePipelineStageDesc() {

}

unsigned int BasePipelineStageDesc::GetSizeOfStage() const {
	return PipelineStage::StageSize(type_);
}

Texture* BasePipelineStageDesc::InputTextureByName(const TextureIdent& ident) {
	return input_textures_[input_texture_lookup_[ident]];
}

vector<BasePipelineStageDesc*> BasePipelineStageDesc::GetBaseStages() {
	vector<BasePipelineStageDesc*> result;
	result.push_back(this);
	return result;
}

RenderEntitiesDesc::RenderEntitiesDesc(
	string name,
	PipelineStageType type,
	vector<tuple<string, TextureSignature>> output_textures_descs,
	DepthStencilDescription depth_desc,
	vector<TextureIdent> input_textures_idents,
	D3D11_BLEND_DESC blend_desc,
	PipelineCameraIdent camera_ident)
	: BasePipelineStageDesc(name, type, output_textures_descs, depth_desc, input_textures_idents, blend_desc),
	camera_ident_(camera_ident)
{

}

void RenderEntitiesDesc::ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const {
	new (location)RenderEntities(dev, dev_con, *this);
}

void RenderEntitiesDesc::AllocateAdditionalResources(PipelineTexturePlanner& planner) {
	camera_index_ = planner.RequestCameraIndex(camera_ident_);
	entity_handler_set_ = planner.RequestEntityGroup(name_);
}

ProcessingEffectDesc::ProcessingEffectDesc(
	string name,
	vector<tuple<string, TextureSignature>> output_textures_descs,
	vector<TextureIdent> input_textures_idents,
	D3D11_BLEND_DESC blend_desc,
	PixelShader pixel_shader,
	VertexShader vertex_shader,
	char* settings_buffer,
	size_t settings_buffer_size)
	: BasePipelineStageDesc(name, PST_PROCESSING_EFFECT, output_textures_descs, DepthStencilDescription::Empty(), input_textures_idents, blend_desc),
	pixel_shader_(pixel_shader),
	vertex_shader_(vertex_shader),
	settings_buffer_(settings_buffer),
	settings_buffer_size_(settings_buffer_size)
{

}

void ProcessingEffectDesc::ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const {
	new (location)ProcessingEffect(dev, dev_con, *this);
}

void ProcessingEffectDesc::AllocateAdditionalResources(PipelineTexturePlanner& planner) {

}

TextureCopyDesc::TextureCopyDesc(
	string name,
	vector<tuple<string, TextureSignature>> output_textures_descs,
	vector<TextureIdent> input_textures_idents)
	: BasePipelineStageDesc(name, PST_TEXTURE_COPY, output_textures_descs, DepthStencilDescription::Empty(), input_textures_idents, BasePipelineStageDesc::DefaultBlendState())
{

}

void TextureCopyDesc::ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const {
	new (location)TextureCopy(dev, dev_con, *this);
}

void TextureCopyDesc::AllocateAdditionalResources(PipelineTexturePlanner& planner) {

}

void RepeatedStageDescTmp::UpdateStringWithRepetitionNum(string* mutable_string, unsigned int repetition_number) {
	*mutable_string = *mutable_string + "|" + std::to_string(repetition_number);
}

template<>
void RepeatedStageDescTmp::UpdateSubStageDesc(BasePipelineStageDesc* mutable_stage, unsigned int repetition_number) {
	UpdateStringWithRepetitionNum(&mutable_stage->name_, repetition_number);
	for (TextureIdent& input_texture_ident : mutable_stage->input_textures_idents_) {
		UpdateStringWithRepetitionNum(&input_texture_ident, repetition_number);
	}
	for (tuple<TextureIdent, TextureSignature>& output_texture_desc : mutable_stage->output_textures_descs_) {
		UpdateStringWithRepetitionNum(&std::get<0>(output_texture_desc), repetition_number);
	}
	UpdateStringWithRepetitionNum(&mutable_stage->depth_desc_.depth_texture_ident_, repetition_number);
}

template<>
void RepeatedStageDescTmp::UpdateSubStageDesc(TextureCopyDesc* mutable_stage, unsigned int repetition_number) {
	UpdateSubStageDesc<BasePipelineStageDesc>(mutable_stage, repetition_number);
}

template<>
void RepeatedStageDescTmp::UpdateSubStageDesc(RenderEntitiesDesc* mutable_stage, unsigned int repetition_number) {
	UpdateSubStageDesc<BasePipelineStageDesc>(mutable_stage, repetition_number);
	UpdateStringWithRepetitionNum(&mutable_stage->camera_ident_, repetition_number);
}

template<>
void RepeatedStageDescTmp::UpdateSubStageDesc(ProcessingEffectDesc* mutable_stage, unsigned int repetition_number) {
	UpdateSubStageDesc<BasePipelineStageDesc>(mutable_stage, repetition_number);
}


vector<PipelineStage*> RenderEntitiesDesc::AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const {
	return{ new RenderEntities(dev, dev_con, *this) };
}

vector<PipelineStage*> TextureCopyDesc::AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const {
	return{ new TextureCopy(dev, dev_con, *this) };
}

vector<PipelineStage*> ProcessingEffectDesc::AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const {
	return{ new ProcessingEffect(dev, dev_con, *this) };
}
