#include "stdafx.h"
#include "Actor.h"


Actor::Actor(ResourcePool& resource_pool)
	: resource_pool_(resource_pool)
{
}


Actor::~Actor()
{
}

void Actor::LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format) {
	models = resource_pool_.LoadModelAsParts(file_name, output_format);
}

void Actor::CreateComponents() {

}