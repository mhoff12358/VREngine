#pragma once
#include "stdafx.h"

#include "VRBackend/ResourcePool.h"
#include "Shmactor.h"
#include "Component.h"
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

class NewGraphicsObjectCommand {
public:
	DECLARE_COMMAND(NewGraphicsObjectCommand, CREATE);
	DECLARE_COMMAND(NewGraphicsObjectCommand, PLACE_COMPONENT);
	DECLARE_COMMAND(NewGraphicsObjectCommand, SET_SHADER_VALUES);
	DECLARE_COMMAND(NewGraphicsObjectCommand, SET_ENTITY_DRAWN);
};

namespace commands {

class CreateNewGraphicsObject : public CommandArgs {
public:
	CreateNewGraphicsObject(string entity_group_name, vector<EntitySpecification> entities, vector<ComponentInfo> components) :
		CommandArgs(NewGraphicsObjectCommand::CREATE),
		entity_group_name_(entity_group_name),
		entities_(entities),
		components_(components)
	{}

	string entity_group_name_;
	vector<EntitySpecification> entities_;
	vector<ComponentInfo> components_;
};

class PlaceNewComponent : public CommandArgs {
public:
	PlaceNewComponent(string component_name, Pose pose) :
		CommandArgs(NewGraphicsObjectCommand::PLACE_COMPONENT),
		pose_(pose),
		component_name_(component_name)
	{}

	string component_name_;
	Pose pose_;
};

}  // namespace commands

namespace actors {

class NewGraphicsObject : public Shmactor {
public:
	NewGraphicsObject() : Shmactor(), first_entity_id_(0), number_of_entities_(0) {}

	void HandleCommand(const CommandArgs& args) override;

private:
	void InitializeEntities(const commands::CreateNewGraphicsObject& args);
	void PlaceComponent(const commands::PlaceNewComponent& args);

	Component* GetTransformByName(const string& transform_name);

	unsigned int first_entity_id_;
	unsigned int number_of_entities_;
	map<string, unsigned int> entity_lookup_;
	vector<unique_ptr<ConstantBuffer>> constant_buffers_;

	vector<Component> transforms_;
	map<string, size_t> transform_lookup_;
};

}  // actors
}  // game_scene
