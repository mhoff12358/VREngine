#pragma once

#include "stdafx.h"

#include "ResourcePool.h"
#include "ObjLoader.h"
#include "InteractableCollection.h"
#include "BeginDirectx.h"
#include "LuaGameScripting/Environment.h"
#include "LuaGameScripting/InteractableObject.h"
#include "LuaGameScripting/MemberFunctionWrapper.h"

#include "Component.h"
#include "LuaDirectX.h"

class Actor
{
public:
	Actor(ResourcePool& resource_pool);
	~Actor();

	void InitializeFromLuaScript(const string& script_name, const VRBackendBasics& graphics_objects, InteractableCollection& interactable_collection, ConstantBuffer* shader_settings = NULL);

	void LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format);
	void CreateComponents(EntityHandler& entity_handler, ID3D11Device* device_interface, const multimap<string, vector<string>>& parentage);

	void XM_CALLCONV SetComponentTransformation(const string& component_name, DirectX::FXMMATRIX new_transformation);

	unsigned int GetShaderSettingsId();

private:
	ResourcePool& resource_pool_;

	map<string, Model> models_;
	map<string, unsigned int> component_lookup_;
	unsigned int shader_settings_entity_id_;
	//multimap<string, LookInteractable*> transformation_mapped_interactables_;
	vector<Component> components_;

public:
	// All the lua interaction callbacks, as well as the interface
	Lua::InteractableObject lua_interface_;

	// Arguments are <string component_name>
	int ClearComponentTransformation(lua_State* L);
	// Arguments are <string component_name, table matrix_description>
	int ApplyToComponentTransformation(lua_State* L);
};

