#include "stdafx.h"
#include "Registry.h"

namespace game_scene {

RegistryMap& QueryRegistry::GetRegistry() {
	static RegistryMap mapping;
	if (override_registry_) {
		return *override_registry_;
	}
	return mapping;
}

void QueryRegistry::SetRegistry(RegistryMap& registry) {
	override_registry_ = &registry;
}

RegistryMap& CommandRegistry::GetRegistry() {
	static RegistryMap mapping;
	if (override_registry_) {
		return *override_registry_;
	}
	return mapping;
}

void CommandRegistry::SetRegistry(RegistryMap& registry) {
	override_registry_ = &registry;
}

void RegistryMap::AddRegistrationsFrom(RegistryMap& other) {
	for (const string& registration : other.GetRegistries()) {
		Register(registration);
	}
}

RegistryMap* QueryRegistry::override_registry_ = nullptr;
RegistryMap* CommandRegistry::override_registry_ = nullptr;

}  // game_scene