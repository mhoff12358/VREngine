#pragma once

#include "stdafx.h"

#include <string>
#include <map>

#include "ResourcePool.h"
#include "ObjLoader.h"

using std::string;
using std::map;

class Actor
{
public:
	Actor(ResourcePool& resource_pool);
	~Actor();

	void LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format);

	void CreateComponents();

private:
	ResourcePool& resource_pool_;

	std::map<std::string, Model> models;
};

