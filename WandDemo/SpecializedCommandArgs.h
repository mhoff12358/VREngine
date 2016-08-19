#pragma once

#include "CommandArgs.h"
#include "InputHandler.h"
#include "ResourcePool.h"

namespace game_scene {
namespace commands {

class InputCommandType : public CommandType {
public:
	enum InputCommandTypeId : int {
		TICK = INPUT,
		INPUT_UPDATE,
	};
};

class TimeTick : public CommandArgs {
public:
	explicit TimeTick(const int duration)
		: CommandArgs(InputCommandType::TICK), duration_(duration) {
	}

	const int duration_;
};

class InputUpdate : public CommandArgs {
public:
	explicit InputUpdate(const InputHandler* input)
		: CommandArgs(InputCommandType::INPUT_UPDATE), input_(*input) {
	}
	const InputHandler& input_;
};

class GraphicsCommandType : public CommandType {
public:
	enum GraphicsCommandTypeId : int {
		REQUIRE_RESOURCE = GRAPHICS,
		CREATE_COMPONENTS,
		CREATE_COMPONENTS_ASSUME_RESOURCES,
		PLACE_COMPONENT,
		SET_SHADER_VALUES,
	};
};

class ComponentPlacement : public CommandArgs {
public:
	ComponentPlacement(string component_name, DirectX::XMMATRIX transformation) :
		CommandArgs(GraphicsCommandType::PLACE_COMPONENT),
		component_name_(component_name),
		transformation_(transformation) {}

	string component_name_;
	DirectX::XMMATRIX transformation_;
};

}  // commands
}  // game_scene