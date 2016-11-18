#pragma once
#include "stdafx.h"

#include "VRBackend/ResourcePool.h"
#include "ShaderSettings.h"
#include "VRBackend/ShaderStages.h"
#include "VRBackend/TextureView.h"

namespace game_scene {

struct NewIndividualTextureDetails {
	NewIndividualTextureDetails() :
		ident_(""), stages_(), shader_resource_slot_(0), sampler_slot_(0) {}
	NewIndividualTextureDetails(
		string ident, ShaderStages stages,
		int shader_resource_slot, int sampler_slot) :
		ident_(std::move(ident)), stages_(stages),
		shader_resource_slot_(shader_resource_slot),
		sampler_slot_(sampler_slot) {}

	string ident_;
	ShaderStages stages_;
	int shader_resource_slot_;
	int sampler_slot_;
};

struct NewTextureDetails {
	NewTextureDetails() {}
	NewTextureDetails(const vector<NewIndividualTextureDetails>& texture_details) {
		for (const NewIndividualTextureDetails& detail : texture_details) {
			idents_.push_back(detail.ident_);
			stages_.push_back(detail.stages_);
			shader_resource_slots_.push_back(detail.shader_resource_slot_);
			sampler_slots_.push_back(detail.sampler_slot_);
		}
	}

	bool IsActive() const { return !idents_.empty(); }
	vector<ResourceIdent> ToResourceIdents() const {
		vector<ResourceIdent> resources;
		resources.reserve(idents_.size());
		for (const string& ident : idents_) {
			resources.emplace_back(ResourceIdent::TEXTURE, ident);
		}
		return resources;
	}

	vector<TextureView> GetTextureViews(ResourcePool& resources) const {
		vector<TextureView> views;
		views.reserve(idents_.size());
		for (size_t i = 0; i < idents_.size(); i++) {
			views.push_back(TextureView(
				shader_resource_slots_[i],
				sampler_slots_[i],
				stages_[i],
				resources.LoadTexture(idents_[i])));
		}
		return views;
	}

	vector<string> idents_;
	vector<ShaderStages> stages_;
	vector<int> shader_resource_slots_;
	vector<int> sampler_slots_;
};

struct NewShaderDetails {
	NewShaderDetails();
	NewShaderDetails(const vector<ShaderIdentifier>& idents);

	bool IsActive() const {
		bool is_active = false;
		for (const ShaderIdentifier& ident : idents_) {
			is_active |= !ident.IsEmpty();
		}
		return is_active;
	}

	Shaders GetShaders(ResourcePool& resources) const {
		return Shaders(
			idents_[ShaderStage::VERTEX_NUMBER].IsEmpty() ? VertexShader() : resources.LoadVertexShader(idents_[ShaderStage::VERTEX_NUMBER]),
			idents_[ShaderStage::PIXEL_NUMBER].IsEmpty() ? PixelShader() : resources.LoadPixelShader(idents_[ShaderStage::PIXEL_NUMBER]),
			idents_[ShaderStage::GEOMETRY_NUMBER].IsEmpty() ? GeometryShader(false) : resources.LoadGeometryShader(idents_[ShaderStage::GEOMETRY_NUMBER])
		);
	}

	ShaderStages GetShaderStages() const;

	array<ShaderIdentifier, ShaderStage::NUMBER_STAGES> idents_;
};

struct NewModelDetails {
	NewModelDetails() {}
	NewModelDetails(const ModelIdentifier& ident, const ObjLoader::OutputFormat& format = ObjLoader::default_output_format)
		: ident_(ident), format_(format) {}
	NewModelDetails(const ModelIdentifier& ident, std::shared_ptr<ModelGenerator> model_generator)
		: ident_(ident), model_generator_(model_generator) {
	}

	bool IsActive() const {
		return !ident_.GetFileName().empty();
	}

	ResourceIdent ToResourceIdent() const {
		if (IsActive()) {
			return ResourceIdent(ResourceIdent::MODEL, ident_.GetAsString());
		} else {
			return ResourceIdent();
		}
	}

	Model GetModel(ResourcePool& resources) const {
		if (IsActive()) {
			if (model_generator_) {
				return resources.LoadModelFromGenerator(ident_, std::move(*model_generator_));
			} else if (format_) {
				return resources.LoadModelFromFile(ident_, *format_);
			} else {
				return resources.LoadExistingModel(ident_);
			}
		} else {
			return Model();
		}
	}

	ModelIdentifier ident_;
	optional<ObjLoader::OutputFormat> format_;
	std::shared_ptr<ModelGenerator> model_generator_ = nullptr;
};

struct EntityInfo {
	string entity_name_;
	NewModelDetails model_;
	NewTextureDetails textures_;
	NewShaderDetails shaders_;

	ShaderSettingsValue intial_settings_value_;
};

}  // game_scene
