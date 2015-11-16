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

class ActorHandler;

class Actor
{
public:
	Actor(const Identifier& ident, ResourcePool& resource_pool, EntityHandler& entity_handler);
	~Actor();

	void InitializeFromLuaScript(lua_State* L, const VRBackendBasics& graphics_objects, ActorHandler* actor_handler, const string& script_name, const Identifier& ident);

	void LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format);
	map<string, vector<Model>> CreateComponents(ID3D11Device* device_interface, const multimap<string, vector<string>>& parentage);

	void XM_CALLCONV SetComponentTransformation(const string& component_name, DirectX::FXMMATRIX new_transformation);

	unsigned int GetShaderSettingsId();

	vector<Component> components_;
	map<string, unsigned int> component_lookup_;
	vector<unsigned int> shader_settings_entity_ids_;

private:
	ResourcePool& resource_pool_;
	EntityHandler& entity_handler_;
	Identifier ident_;
	map<string, Model> models_;
	string model_resource_id_;
	ObjLoader::OutputFormat output_format_;
	
public:
	// All the lua interaction callbacks, as well as the interface
	Lua::InteractableObject lua_interface_;

	// Arguments are <string component_name>
	int ClearComponentTransformation(lua_State* L);
	// Arguments are <string component_name, table matrix_description>
	int ApplyToComponentTransformation(lua_State* L);
	// Arguments are <string component_name, table matrix_description>
	int SetComponentTransformation(lua_State* L);
	// Arguments are <size_t settings_number, string new_shader_name>
	int SetShader(lua_State* L);
	// Arguments are <size_t settings_number, array of array of floats>
	int SetConstantBuffer(lua_State* L);
	// Arguments are <string component_name, bool draw>
	int SetEnabled(lua_State* L);
	// Arguments are
	int SetVertices(lua_State* L);
};

