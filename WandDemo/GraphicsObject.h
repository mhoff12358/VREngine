#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "ResourcePool.h"
#include "Component.h"
#include "SpecializedActors.h"

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

class ShaderSettingsFormat : public vector<tuple<string, int>> {
public:
	template<typename... Args>
	ShaderSettingsFormat(Args... args) : vector<tuple<string, int>>(args...) {}

	bool ShouldCreateBuffer() { return !empty(); }
};

class ShaderSettingsValue : public vector<vector<float>> {
public:
	template<typename... Args>
	ShaderSettingsValue(Args... args) : vector<vector<float>>(args...) {}

	void operator=(const vector<vector<float>>& other) {
		*this = ShaderSettingsValue(other);
	}

	ShaderSettingsFormat GetFormat() const {
		ShaderSettingsFormat format;
		for (const vector<float>& sub_array : *this) {
			format.push_back(tuple<string, int>("float", sub_array.size()));
		}
		return format;
	}

	vector<vector<float>> GetValue() const { return *this; }

	void SetIntoConstantBuffer(ConstantBuffer* buffer) const {
		float* raw_buffer = static_cast<float*>(buffer->EditBufferData(true));
		for (const vector<float>& next_data_chunk : GetValue()) {
			memcpy(raw_buffer, next_data_chunk.data(), sizeof(float) * next_data_chunk.size());
			raw_buffer = raw_buffer + next_data_chunk.size();
		}
	}
};

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
		shader_definitions_.resize(static_cast<unsigned char>(ShaderStage::NUM_STAGES));
		shader_definitions_[static_cast<unsigned char>(ShaderStage::VERTEX)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStage::VERTEX)] ? "VShader" : "");
		shader_definitions_[static_cast<unsigned char>(ShaderStage::GEOMETRY)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStage::GEOMETRY)] ? "GShader" : "");
		shader_definitions_[static_cast<unsigned char>(ShaderStage::PIXEL)] =
			pair<string, string>(filename, add_stage[static_cast<unsigned char>(ShaderStage::PIXEL)] ? "PShader" : "");
	}

	bool ShouldLoadStage(ShaderStage shader_stage) const { return !shader_definitions_[static_cast<unsigned char>(shader_stage)].second.empty(); }
	string StageFileName(ShaderStage shader_stage) const { return shader_definitions_[static_cast<unsigned char>(shader_stage)].first; }
	string StageFunctionName(ShaderStage shader_stage) const { return shader_definitions_[static_cast<unsigned char>(shader_stage)].second; }

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