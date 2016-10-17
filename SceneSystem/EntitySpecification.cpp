#include "stdafx.h"
#include "EntitySpecification.h"
#include "GraphicsResources.h"

namespace game_scene {
	EntitySpecification& EntitySpecification::SetShaderSettingsValue(ShaderSettingsValue shader_settings_value) {
		shader_settings_value_ = move(shader_settings_value);
		shader_settings_format_ = shader_settings_value_.GetFormat();
		return *this;
	}
	EntitySpecification& EntitySpecification::SetShaderSettingsFormat(ShaderSettingsFormat shader_settings_format) {
		assert(shader_settings_value_.empty());
		shader_settings_format_ = move(shader_settings_format);
		return *this;
	}
	EntitySpecification& EntitySpecification::SetShaders(NewShaderDetails shader_details) {
		shader_details_ = move(shader_details);
		return *this;
	}
	EntitySpecification& EntitySpecification::SetShaderStages(ShaderStages shader_stages) {
		shader_stages_ = shader_stages;
		return *this;
	}

	EntitySpecification& EntitySpecification::SetTextures(const vector<NewIndividualTextureDetails>& texture_details) {
		texture_details_ = NewTextureDetails(texture_details);
		return *this;
	}

	EntitySpecification& EntitySpecification::SetModel(NewModelDetails model_details) {
		model_details_ = move(model_details);
		return *this;
	}
	EntitySpecification& EntitySpecification::SetComponent(string component_name) {
		component_name_ = move(component_name);
		return *this;
	}
}  /// game_scene