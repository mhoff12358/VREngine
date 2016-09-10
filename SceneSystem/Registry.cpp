#include "stdafx.h"
#include "Registry.h"

namespace game_scene {

RegistryMap& QueryRegistry::GetRegistry() {
	static RegistryMap mapping;
	return mapping;
}

RegistryMap& CommandRegistry::GetRegistry() {
	static RegistryMap mapping;
	return mapping;
}

}  // game_scene