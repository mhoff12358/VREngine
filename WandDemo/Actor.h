#pragma once

#include "stdafx.h"

#include "ResourcePool.h"
#include "ObjLoader.h"

#include "Component.h"

class Actor
{
public:
	Actor(ResourcePool& resource_pool);
	~Actor();

	void LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format);
	map<string, unsigned int> CreateComponents(EntityHandler& entity_handler, ID3D11Device* device_interface, const multimap<string, vector<string>>& parentage);

	void XM_CALLCONV SetComponentTransformation(unsigned int component_index, DirectX::FXMMATRIX new_transformation);

private:
	ResourcePool& resource_pool_;

	map<string, Model> models_;
	vector<Component> components_;
};

