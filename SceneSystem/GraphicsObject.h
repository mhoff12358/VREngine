#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "VRBackend/ResourcePool.h"
#include "Component.h"
#include "GraphicsResources.h"
#include "ShaderSettings.h"

namespace game_scene {

class GraphicsObjectCommand {
public:
	DECLARE_COMMAND(GraphicsObjectCommand, REQUIRE_RESOURCE);
	DECLARE_COMMAND(GraphicsObjectCommand, CREATE_COMPONENTS);
	DECLARE_COMMAND(GraphicsObjectCommand, CREATE_COMPONENTS_ASSUME_RESOURCES);
	DECLARE_COMMAND(GraphicsObjectCommand, PLACE_COMPONENT);
	DECLARE_COMMAND(GraphicsObjectCommand, SET_SHADER_VALUES);
	DECLARE_COMMAND(GraphicsObjectCommand, SET_COMPONENT_DRAWN);
};

namespace commands {

class ComponentPlacement : public CommandArgs {
public:
	ComponentPlacement(string component_name, DirectX::XMMATRIX transformation) :
		CommandArgs(GraphicsObjectCommand::PLACE_COMPONENT),
		component_name_(component_name),
		transformation_(transformation) {}

	string component_name_;
	DirectX::XMMATRIX transformation_;
};

class ComponentDrawnState : public CommandArgs {
public:
	ComponentDrawnState(string component_name, bool is_drawn) :
		CommandArgs(GraphicsObjectCommand::SET_COMPONENT_DRAWN),
		component_name_(component_name), is_drawn_(is_drawn) {}

	string component_name_;
	bool is_drawn_;
};

}  // commands

namespace actors {

class TextureDetails {
public:
	TextureDetails(string name, bool use_in_vertex, bool use_in_pixel)
		: name_(name), use_in_vertex_(use_in_vertex), use_in_pixel_(use_in_pixel) {}
	string name_;
	bool use_in_vertex_;
	bool use_in_pixel_;
};

class ShaderFileDefinition {
public:
	ShaderFileDefinition() {}
	explicit ShaderFileDefinition(string filename) : ShaderFileDefinition(filename, {true, false, true}) {}
	ShaderFileDefinition(string filename, array<bool, 3> add_stage) {
		shader_definitions_.resize(static_cast<unsigned char>(ShaderStages::NUMBER_STAGES));
		shader_definitions_[static_cast<unsigned char>(ShaderStages::VERTEX_NUMBER)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStages::VERTEX_NUMBER)] ? "VShader" : "");
		shader_definitions_[static_cast<unsigned char>(ShaderStages::GEOMETRY_NUMBER)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStages::GEOMETRY_NUMBER)] ? "GShader" : "");
		shader_definitions_[static_cast<unsigned char>(ShaderStages::PIXEL_NUMBER)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStages::PIXEL_NUMBER)] ? "PShader" : "");
	}

	bool ShouldLoadStage(ShaderStage shader_stage) const { return !shader_definitions_[shader_stage.ToStageNumber()].second.empty(); }
	string StageFileName(ShaderStage shader_stage) const { return shader_definitions_[shader_stage.ToStageNumber()].first; }
	string StageFunctionName(ShaderStage shader_stage) const { return shader_definitions_[shader_stage.ToStageNumber()].second; }

private:
	vector<pair<string, string>> shader_definitions_;
};

class ComponentHeirarchy {
public:
	ComponentHeirarchy() {}
	ComponentHeirarchy(string name, vector<tuple<string, ObjLoader::OutputFormat>> models, vector<ComponentHeirarchy> children)
		: name_(name), models_(models), children_(children) {}

	void GetRequiredResources(vector<ResourceIdent>* resources) const;
	int GetNumberOfComponents() const;

	// Heirarchy values
	string name_;
	vector<ComponentHeirarchy> children_;
	
	// Component values
	vector<tuple<string, ObjLoader::OutputFormat>> models_;
	vector<TextureDetails> textures_;
	ShaderFileDefinition shader_file_definition_;
	VertexType vertex_shader_input_type_;
	ShaderSettingsValue shader_settings_;
	string entity_group_;
};

class GraphicsObjectDetails {
public:
	GraphicsObjectDetails() {}
	GraphicsObjectDetails(ComponentHeirarchy heirarchy) : heirarchy_(heirarchy) {}

	ComponentHeirarchy heirarchy_;
};

class GraphicsObject : public Shmactor {
public:
	GraphicsObject() {}
	GraphicsObject(const GraphicsObject&) = delete;
	GraphicsObject operator=(const GraphicsObject&) = delete;

	void HandleCommand(const CommandArgs& args) override;

protected:
	// Creation methods.
	void RequestResourcesAndCreateComponents(const GraphicsObjectDetails& details);
	void CreateSettingsEntity(
		actors::GraphicsResources& graphics_resources,
		const ComponentHeirarchy& heirarchy, unsigned int reserved_space);
	void CreateComponents(const GraphicsObjectDetails& details);
	void CreateIndividualComponent(
		actors::GraphicsResources& graphics_resources,
		const ComponentHeirarchy& heirarchy, unsigned int reserved_space);

	// Update methods.
	void PlaceComponent(const commands::ComponentPlacement& placement);
	void SetShaderSettingsValue(string component_name, const ShaderSettingsValue& value);
	void SetComponentIsDrawn(const commands::ComponentDrawnState& component_drawn_state);

	// Each tuple is a component, an unsigned int that is the entity numbers of
	// the settings entities, and a unique_ptr holding the settings' constant
	// buffer.
	vector<tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>> components_;
	map<string, unsigned int> component_lookup_;
	tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>&
		LookupComponent(string component_name);
};

}  // actors
}  // game_scene