#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "ResourcePool.h"
#include "Component.h"
#include "SpecializedActors.h"

namespace game_scene {
namespace actors {

class ShaderSettingsFormat : public vector<tuple<string, int>> {
public:
	template<typename... Args>
	ShaderSettingsFormat(Args... args) : vector<tuple<string, int>>(args...) {}
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
	};

	vector<vector<float>> GetValue() const { return *this; };
};

class TextureDetails {
public:
	TextureDetails(string name, bool use_in_vertex, bool use_in_pixel)
		: name_(name), use_in_vertex_(use_in_vertex), use_in_pixel_(use_in_pixel) {}
	string name_;
	bool use_in_vertex_;
	bool use_in_pixel_;
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
	string shader_name_;
	ShaderSettingsValue shader_settings_;
	unsigned int entity_group_;
};

class GraphicsObjectDetails {
public:
	ComponentHeirarchy heirarchy_;
};

class GraphicsObject : public Shmactor {
public:
	void HandleCommand(const CommandArgs& args);

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

	// Each tuple is a component, an unsigned int that is the entity numbers of
	// the settings entities, and a unique_ptr holding the settings' constant
	// buffer.
	vector<tuple<Component, unique_ptr<ConstantBuffer>, vector<unsigned int>>> components_;
	map<string, unsigned int> component_lookup_;
};

}  // actors
}  // game_scene