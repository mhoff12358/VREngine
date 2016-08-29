#pragma once

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include "stl.h"
#include "Texture.h"
#include "PipelineStage.h"
#include "Shaders.h"

typedef string TextureIdent;

class PipelineTexturePlanner;

struct DepthStencilDescription {
	DepthStencilDescription(
		TextureIdent depth_texture_ident,
		D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc)
		: depth_texture_ident_(depth_texture_ident),
		depth_stencil_state_desc_(depth_stencil_state_desc) {}
	DepthStencilDescription(
		TextureIdent depth_texture_ident)
		: depth_texture_ident_(depth_texture_ident) {}

	static DepthStencilDescription Empty() { return DepthStencilDescription(""); }

	TextureIdent depth_texture_ident_;
	D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc_;
};

struct TextureSignature {
	TextureSignature(D3D11_TEXTURE2D_DESC texture_desc)
		: texture_desc_(texture_desc) {}

	D3D11_TEXTURE2D_DESC texture_desc_;

	friend bool operator<(const TextureSignature& l, const TextureSignature& r) {
		return memcmp(&l.texture_desc_, &r.texture_desc_, sizeof(D3D11_TEXTURE2D_DESC)) < 0;
	}

	friend bool operator==(const TextureSignature& l, const TextureSignature& r) {
		return memcmp(&l.texture_desc_, &r.texture_desc_, sizeof(D3D11_TEXTURE2D_DESC)) == 0;
	}
};

class BasePipelineStageDesc;

class PipelineStageDesc  {
public:
	PipelineStageDesc();

	virtual unsigned int GetSizeOfStage() const = 0;
	virtual void ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const = 0;
	virtual vector<PipelineStage*> AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const = 0;
	virtual void AllocateAdditionalResources(PipelineTexturePlanner& planner) = 0;
	virtual vector<BasePipelineStageDesc*> GetBaseStages() = 0;

	static string GetBaseName(const string name);
};

class BasePipelineStageDesc : public PipelineStageDesc {
public:
	BasePipelineStageDesc(
		string name,
		PipelineStageType type,
		vector<tuple<string, TextureSignature>> output_textures_descs,
		DepthStencilDescription depth_desc,
		vector<TextureIdent> input_textures_idents,
		D3D11_BLEND_DESC blend_desc);

	virtual ~BasePipelineStageDesc();

	unsigned int GetSizeOfStage() const override;
	vector<BasePipelineStageDesc*> GetBaseStages() override;

	Texture* InputTextureByName(const TextureIdent& ident);

	// Attributes provided by the client to configure the pipeline
	PipelineStageType type_;
	string name_;
	D3D11_BLEND_DESC blend_desc_;
	vector<tuple<TextureIdent, TextureSignature>> output_textures_descs_;
	vector<TextureIdent> input_textures_idents_;
	DepthStencilDescription depth_desc_;

	// After parsing the input and creating resources, information about resources that needs to be filled in
	int entity_handler_set_;
	vector<Texture*> output_textures_;
	vector<Texture*> input_textures_;
	map<TextureIdent, unsigned int> input_texture_lookup_;
	ID3D11DepthStencilView* depth_stencil_view_;
	ID3D11DepthStencilState* depth_stencil_state_;

protected:
	static D3D11_BLEND_DESC DefaultBlendState();
};

class RenderEntitiesDesc : public BasePipelineStageDesc {
public:
	RenderEntitiesDesc(
		string name,
		PipelineStageType type,
		vector<tuple<string, TextureSignature>> output_textures_descs,
		DepthStencilDescription depth_desc,
		vector<TextureIdent> input_textures_idents,
		D3D11_BLEND_DESC blend_desc,
		PipelineCameraIdent camera_ident);

	virtual void ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const override;
	virtual void AllocateAdditionalResources(PipelineTexturePlanner& planner) override;
	virtual vector<PipelineStage*> AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const override;

	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetViewProjectionMatrix() const;
	const DirectX::XMMATRIX& GetOrientationProjectionMatrix() const;

	// Configuration variables
	PipelineCameraIdent camera_ident_;

	// Values to be filled in based on configuration variables
	const DirectX::XMMATRIX* view_matrix_;
	const DirectX::XMMATRIX* view_projection_matrix_;
	const DirectX::XMMATRIX* orientation_projection_matrix_;
};

class TextureCopyDesc : public BasePipelineStageDesc {
public:
	TextureCopyDesc(
		string name,
		vector<tuple<string, TextureSignature>> output_textures_descs,
		vector<TextureIdent> input_textures_idents);

	virtual void ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const override;
	virtual void AllocateAdditionalResources(PipelineTexturePlanner& planner) override;
	virtual vector<PipelineStage*> AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const override;
};

class ProcessingEffectDesc : public BasePipelineStageDesc {
public:
	ProcessingEffectDesc(
		string name,
		vector<tuple<string, TextureSignature>> output_textures_descs,
		vector<TextureIdent> input_textures_idents,
		D3D11_BLEND_DESC blend_desc,
		PixelShader pixel_shader,
		VertexShader vertex_shader,
		char* settings_buffer,
		size_t settings_buffer_size);

	virtual void ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const override;
	virtual void AllocateAdditionalResources(PipelineTexturePlanner& planner) override;
	virtual vector<PipelineStage*> AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const override;

	char* settings_buffer_;
	unsigned int settings_buffer_size_;

	PixelShader pixel_shader_;
	VertexShader vertex_shader_;
};

class RepeatedStageDescTmp : public PipelineStageDesc {
protected:
	template<typename UpdatedSubStageDesc>
	void UpdateSubStageDesc(UpdatedSubStageDesc* mutable_stage, unsigned int repetition_number);
	void UpdateStringWithRepetitionNum(string* mutable_string, unsigned int repetition_number);
};

template<typename SubStageDesc>
class RepeatedStageDesc : public RepeatedStageDescTmp {
public:
	RepeatedStageDesc(
		unsigned int num_repetitions,
		const SubStageDesc& sub_stage);

	void ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const override;
	void AllocateAdditionalResources(PipelineTexturePlanner& planner) override;
	vector<PipelineStage*> AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const override;
	vector<BasePipelineStageDesc*> GetBaseStages() override;
	unsigned int GetSizeOfStage() const override;

protected:
	vector<unique_ptr<SubStageDesc>> sub_stages_;
};

template<typename SubStageDesc>
RepeatedStageDesc<SubStageDesc>::RepeatedStageDesc(
	unsigned int num_repetitions,
	const SubStageDesc& sub_stage) {
	if (num_repetitions == 0) {
		sub_stages_.emplace_back(new SubStageDesc(sub_stage));
	} else {
		for (unsigned int rep_num = 0; rep_num < num_repetitions; rep_num++) {
			sub_stages_.emplace_back(new SubStageDesc(sub_stage));
			UpdateSubStageDesc<SubStageDesc>(sub_stages_[rep_num].get(), rep_num);
		}
	}
}

template<typename SubStageDesc>
unsigned int RepeatedStageDesc<SubStageDesc>::GetSizeOfStage() const {
	unsigned int total_size = 0;
	for (const unique_ptr<SubStageDesc>& sub_stage : sub_stages_) {
		total_size += sub_stage->GetSizeOfStage();
	}
	return total_size;
}

template<typename SubStageDesc>
void RepeatedStageDesc<SubStageDesc>::ConstructStageInPlace(ID3D11Device* dev, ID3D11DeviceContext* dev_con, char* location) const {
	for (const unique_ptr<SubStageDesc>& sub_stage : sub_stages_) {
		sub_stage->ConstructStageInPlace(dev, dev_con, location);
		location += sub_stage->GetSizeOfStage();
	}
}

template<typename SubStageDesc>
void RepeatedStageDesc<SubStageDesc>::AllocateAdditionalResources(PipelineTexturePlanner& planner) {
	for (const unique_ptr<SubStageDesc>& sub_stage : sub_stages_) {
		sub_stage->AllocateAdditionalResources(planner);
	}
}

template<typename SubStageDesc>
vector<BasePipelineStageDesc*> RepeatedStageDesc<SubStageDesc>::GetBaseStages() {
	vector<BasePipelineStageDesc*> result;
	for (int i = 0; i < sub_stages_.size(); i++) {
		result.push_back(sub_stages_[i].get());
	}
	return result;
}

template<typename SubStageDesc>
vector<PipelineStage*> RepeatedStageDesc<SubStageDesc>::AllocateStage(ID3D11Device* dev, ID3D11DeviceContext* dev_con) const {
	vector<PipelineStage*> sub_stage_ptrs;
	for (const unique_ptr<SubStageDesc>& sub_stage : sub_stages_) {
		vector<PipelineStage*> sub_stages = sub_stage->AllocateStage(dev, dev_con);
		sub_stage_ptrs.insert(sub_stage_ptrs.end(), sub_stages.begin(), sub_stages.end());
	}
	return sub_stage_ptrs;
}