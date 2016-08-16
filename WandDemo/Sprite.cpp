#include "stdafx.h"

#include "Sprite.h"

namespace game_scene {
namespace actors {

GraphicsObjectDetails Sprite::generic_details_;

void Sprite::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case commands::SpriteCommandType::CREATE_SPRITE:
	{
		CreateSprite(dynamic_cast<const commands::SpriteDetails&>(args));
	}
		break;
	case commands::SpriteCommandType::PLACE_SPRITE:
	{
		PlaceSprite(dynamic_cast<const commands::SpritePlacement&>(args));
	}
		break;
	default:
		GraphicsObject::HandleCommand(args);
		break;
	}
}

void Sprite::CreateSprite(const commands::SpriteDetails& details) {
	GraphicsObjectDetails specific_details = generic_details_;
	specific_details.heirarchy_.textures_ = {game_scene::actors::TextureDetails(details.texture_name_, false, true)};
	RequestResourcesAndCreateComponents(specific_details);
}

void Sprite::PlaceSprite(const commands::SpritePlacement& placement) {
	PlaceComponent(commands::ComponentPlacement("point", Pose(placement.location_, Quaternion::Identity()).GetMatrix()));
	ShaderSettingsValue value = vector<vector<float>>({ { placement.size_[0], placement.size_[1] } });
	SetShaderSettingsValue("point", value);
}

void Sprite::Init() {
	generic_details_ = GraphicsObjectDetails(ComponentHeirarchy(
		"point",
		{
			{
				ResourcePool::GetConstantModelName("point"),
				ObjLoader::OutputFormat(
				ModelModifier({0, 1, 2}, {1, 1, 1}, {false, true}),
					ObjLoader::vertex_type_location,
					false)
			},
		},
		{}
		));
	generic_details_.heirarchy_.shader_name_ = "sprite_textured.hlsl";
	generic_details_.heirarchy_.vertex_shader_input_type_ = ObjLoader::vertex_type_location;
	generic_details_.heirarchy_.geometry_shader_name_ = "sprite_geo.hlsl";
	generic_details_.heirarchy_.entity_group_ = "basic";
	generic_details_.heirarchy_.shader_settings_ = {
		{1.0f},
	};
}

}  // actors
}  // game_scene
