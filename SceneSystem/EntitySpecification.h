#pragma once
#include "stdafx.h"

#include "ResourceDefinitions.h"

namespace game_scene {

	class EntitySpecification {
	public:
		EntitySpecification() {}
		EntitySpecification(string entity_name) : entity_name_(move(entity_name)) {}

		EntitySpecification& SetShaderStages(ShaderStages shader_stages);
		EntitySpecification& SetShaderSettingsValue(ShaderSettingsValue shader_settings_value);
		EntitySpecification& SetShaderSettingsFormat(ShaderSettingsFormat shader_settings_format);

		EntitySpecification& SetShaders(NewShaderDetails shader_details);

		EntitySpecification& SetTextures(const vector<NewIndividualTextureDetails>& texture_details);

		EntitySpecification& SetModel(NewModelDetails model_details);
		EntitySpecification& SetComponent(string component_name);

		// The name the entity will be referred by.
		// Can be empty, which makes it just number referencable.
		string entity_name_;

		NewTextureDetails texture_details_;

		NewShaderDetails shader_details_;
		// If the format is set but not the value then the shader will have no value set.
		ShaderSettingsValue shader_settings_value_;
		ShaderSettingsFormat shader_settings_format_;
		// If the shader details are set then this will be set automatically.
		// Otherwise it must be set when the shader settings format is set.
		ShaderStages shader_stages_;

		// The name of the component to use for placement. This sets the object settings buffer.
		string component_name_;
		NewModelDetails model_details_;
	};
}  // game_scene
#pragma once
