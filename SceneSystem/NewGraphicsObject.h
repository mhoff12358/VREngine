#pragma once
#include "stdafx.h"

#include "CommandArgs.h"
#include "EntitySpecification.h"
#include "Actor.h"
#include "Component.h"
#include "GraphicsResources.h"

namespace game_scene {

class NewGraphicsObjectCommand {
public:
	DECLARE_COMMAND(NewGraphicsObjectCommand, CREATE);
	DECLARE_COMMAND(NewGraphicsObjectCommand, PLACE_COMPONENT);
	DECLARE_COMMAND(NewGraphicsObjectCommand, SET_ENTITY_SHADER_VALUES);
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

class SetEntityShaderValues : public CommandArgs {
public:
	SetEntityShaderValues(string entity_name, ShaderSettingsValue value) :
		CommandArgs(NewGraphicsObjectCommand::SET_ENTITY_SHADER_VALUES),
		entity_name_(entity_name),
		value_(std::move(value)) {}

	string entity_name_;
	ShaderSettingsValue value_;
};

}  // namespace commands

namespace actors {

class NewGraphicsObjectImpl {
public:
	NewGraphicsObjectImpl() : entities_(0, 0) {}

	void InitializeEntities(const commands::CreateNewGraphicsObject& args, actors::GraphicsResources& graphics_resources);
	void PlaceComponent(const commands::PlaceNewComponent& args);
	void SetShaderValues(const commands::SetEntityShaderValues& args, actors::GraphicsResources& graphics_resources);

private:
	struct EntityRange {
		EntityRange() : first_entity_(0), number_of_entities_(0) {}
		EntityRange(unsigned int first_entity, unsigned int number_of_entities) :
			first_entity_(first_entity), number_of_entities_(number_of_entities) {}
		unsigned int first_entity_;
		unsigned int number_of_entities_;

		EntityRange EvaluateInRange(const EntityRange& outer_range) {
			assert(number_of_entities_ <= (outer_range.number_of_entities_ - first_entity_));
			return EntityRange(first_entity_ + outer_range.first_entity_, number_of_entities_);
		}

		unsigned int GetMainEntity() {
			if (number_of_entities_ == 0) {
				return first_entity_;
			}
			return first_entity_ + number_of_entities_ - 1;
		}

		unsigned int GetTextureEntity(unsigned int texture_index) {
			assert(texture_index < number_of_entities_);
			return first_entity_ + texture_index;
		}
	};

	Component* GetTransformByName(const string& transform_name);
	EntityRange GetEntitiesByName(const string& entity_name);

	EntityRange entities_;
	map<string, EntityRange> entity_lookup_;
	vector<unique_ptr<ConstantBuffer>> constant_buffers_;

	vector<Component> transforms_;
	map<string, size_t> transform_lookup_;
};

template<typename ActorBase>
class NewGraphicsObjectTemp : public ActorBase {
public:
	void HandleCommand(const CommandArgs& args) {
		switch (args.Type()) {
		case CommandType::ADDED_TO_SCENE:
			break;
		case NewGraphicsObjectCommand::CREATE:
			impl_.InitializeEntities(dynamic_cast<const commands::CreateNewGraphicsObject&>(args), GraphicsResources::GetGraphicsResources(&GetScene()));
			return;
		case NewGraphicsObjectCommand::PLACE_COMPONENT:
			impl_.PlaceComponent(dynamic_cast<const commands::PlaceNewComponent&>(args));
			return;
		case NewGraphicsObjectCommand::SET_ENTITY_SHADER_VALUES:
			impl_.SetShaderValues(dynamic_cast<const commands::SetEntityShaderValues&>(args), GraphicsResources::GetGraphicsResources(&GetScene()));
			return;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	string GetName() const {
		return "NewGraphicsObject-" + ActorBase::GetName();
	}

private:
	NewGraphicsObjectImpl impl_;
};

typedef ActorAdapter<NewGraphicsObjectTemp<ActorImpl>> NewGraphicsObject;

}  // actors
}  // game_scene
