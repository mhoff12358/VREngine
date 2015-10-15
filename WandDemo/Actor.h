#pragma once

#include "stdafx.h"

#include "ResourcePool.h"
#include "ObjLoader.h"
#include "InteractableCollection.h"
#include "LuaGameScripting/InteractableObject.h"
#include "LuaGameScripting/Environment.h"

#include "Component.h"

class Actor
{
public:
	Actor(ResourcePool& resource_pool);
	~Actor();

	void InitializeFromLuaScript(const string& script_name, EntityHandler& entity_handler, ID3D11Device* device_interface, InteractableCollection& interactable_collection);

	void LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format);
	void CreateComponents(EntityHandler& entity_handler, ID3D11Device* device_interface, const multimap<string, vector<string>>& parentage);

	void XM_CALLCONV SetComponentTransformation(const string& component_name, DirectX::FXMMATRIX new_transformation);

private:
	ResourcePool& resource_pool_;

	map<string, Model> models_;
	map<string, unsigned int> component_lookup_;
	multimap<string, LookInteractable*> transformation_mapped_interactables_;
	vector<Component> components_;
	Lua::Environment lua_environment_;
	vector<Lua::InteractableObject> lua_interfaces_;
};

