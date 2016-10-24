#pragma once
#include "stdafx.h"

#include "VRBackend/ResourcePool.h"
#include "ShaderSettings.h"

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
	NewShaderDetails() {}
	NewShaderDetails(const string& filename, VertexType vertex_type, bool include_geometry_shader = true)
		: vertex_type_(std::move(vertex_type)) {
		idents_[0] = filename;
		idents_[1] = include_geometry_shader ? filename : ResourceIdentifier::GetConstantModelName("unset_geometry_shader");
		idents_[2] = filename;
	};
	NewShaderDetails(const string& filename, VertexType vertex_type, ShaderStages stages)
		: vertex_type_(std::move(vertex_type)) {
		idents_[0] = stages.IsVertexStage() ? filename : "";
		idents_[1] = stages.IsGeometryStage() ? filename : "";
		idents_[2] = stages.IsPixelStage() ? filename : "";
	}

	bool IsActive() const {
		bool is_active = false;
		for (const string& ident : idents_) {
			is_active |= !ident.empty();
		}
		return is_active;
	}
	vector<ResourceIdent> ToResourceIdents() const {
		vector<ResourceIdent> resources;
		if (!idents_[0].empty())
			resources.push_back(ResourceIdent(ResourceIdent::VERTEX_SHADER, idents_[0], vertex_type_));
		if (!idents_[1].empty())
			resources.push_back(ResourceIdent(ResourceIdent::GEOMETRY_SHADER, idents_[1]));
		if (!idents_[2].empty())
			resources.push_back(ResourceIdent(ResourceIdent::PIXEL_SHADER, idents_[2]));
		return resources;
	}

	Shaders GetShaders(ResourcePool& resources) const {
		return Shaders(
			idents_[0].empty() ? VertexShader() : resources.LoadVertexShader(idents_[0], vertex_type_),
			idents_[2].empty() ? PixelShader() : resources.LoadPixelShader(idents_[2]),
			idents_[1].empty() ? GeometryShader(false) : resources.LoadGeometryShader(idents_[1])
		);
	}

	array<string, 3> idents_;
	VertexType vertex_type_;
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
