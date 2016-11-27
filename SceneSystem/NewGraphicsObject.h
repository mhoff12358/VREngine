#pragma once
#include "stdafx.h"

#include "CommandArgs.h"
#include "EntitySpecification.h"
#include "Actor.h"
#include "Component.h"

namespace game_scene {

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

class NewGraphicsObject : public Actor {
public:
	NewGraphicsObject() : Actor(), first_entity_id_(0), number_of_entities_(0) {}

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
