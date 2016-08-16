#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "ResourcePool.h"
#include "Component.h"
#include "SpecializedActors.h"
#include "GraphicsObject.h"
#include "Pose.h"

namespace game_scene {
namespace commands {

class SpriteCommandType : public CommandType {
public:
	enum SpriteCommandTypeId : int {
		CREATE_SPRITE = SPRITES,
		PLACE_SPRITE,
	};
};

class SpriteDetails : public CommandArgs {
public:
	SpriteDetails(string texture_name) :
		CommandArgs(SpriteCommandType::CREATE_SPRITE), texture_name_(texture_name) {}

	string texture_name_;
};

class SpritePlacement : public CommandArgs {
public:
	SpritePlacement(Location location, array<float, 2> size) :
		CommandArgs(SpriteCommandType::PLACE_SPRITE), location_(location), size_(size) {}

	Location location_;
	array<float, 2> size_;
};

}  // commands
namespace actors {

class Sprite : public GraphicsObject {
public:
	void HandleCommand(const CommandArgs& args) override;

	static void Init();

protected:
	void CreateSprite(const commands::SpriteDetails& details);
	void PlaceSprite(const commands::SpritePlacement& placement);

	static GraphicsObjectDetails generic_details_;
};

}  // actors
}  // game_scene
