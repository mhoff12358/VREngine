#pragma once
#include "stdafx.h"

#define REGISTER_COMMAND(class_name, command_name) __pragma(warning(push)) __pragma(warning(disable: 4307)) CommandRegistry class_name::REGISTRY_##command_name(#class_name":"#command_name __pragma(warning(pop)))
#define DECLARE_COMMAND(class_name, command_name) __pragma(warning(push)) __pragma(warning(disable: 4307)) static CommandRegistry REGISTRY_##command_name; static constexpr ::game_scene::IdType command_name = FNV(#class_name":"#command_name); __pragma(warning(pop))
#define REGISTER_QUERY(class_name, query_name) __pragma(warning(push)) __pragma(warning(disable: 4307)) QueryRegistry class_name::REGISTRY_##query_name(#class_name":"#query_name __pragma(warning(pop)))
#define DECLARE_QUERY(class_name, query_name) __pragma(warning(push)) __pragma(warning(disable: 4307)) static QueryRegistry REGISTRY_##query_name; static constexpr ::game_scene::IdType query_name = FNV(#class_name":"#query_name); __pragma(warning(pop))

namespace game_scene {

typedef int64_t IdType;

constexpr IdType FNV(const char* string) {
	return *string == '\0' ? 14695981039346656037 : 1099511628211 * (*string ^ FNV(string + 1));
}

class RegistryMap {
public:
	void AddRegistrationsFrom(RegistryMap& other);

	IdType Register(string new_value) {
		const auto& mapping = GetMapping();
		IdType value = FNV(new_value.data());
		const auto& existing_mapping_iter = mapping.find(value);
		if (existing_mapping_iter != mapping.end()) {
			string existing_key = existing_mapping_iter->second;
			if (existing_key != new_value) {
				std::cout << "Hash collision occured in registry" << std::endl;
			}
			return value;
		} else {
			GetRegistries().push_back(new_value);
			GetMapping().insert(make_pair(value, new_value));
		}
		return value;
	}

	IdType IdFromName(const string& name) const {
		return FNV(name.data());
	}

	string LookupName(IdType id) {
		map<IdType, string>& map = GetMapping();
		const auto& iter = map.find(id);
		if (iter == map.end()) {
			return "";
		}
		return iter->second;
	}
	
	string LookupNameConst(IdType id) const {
		if (mapping_ == nullptr) {
			return "";
		}
		const auto& iter = mapping_->find(id);
		if (iter == mapping_->end()) {
			return "";
		}
		return iter->second;
	}

	size_t size() {
		return mapping_->size();
	}

private:
	vector<string>& GetRegistries() {
		if (registries_ == nullptr) {
			registries_ = make_unique<vector<string>>();
		}
		return *registries_;
	}

	map<IdType, string>& GetMapping() {
		if (mapping_ == nullptr) {
			mapping_ = make_unique<map<IdType, string>>();
		}
		return *mapping_;
	}

	unique_ptr<map<IdType, string>> mapping_ = nullptr;
	unique_ptr<vector<string>> registries_ = nullptr;
};

class QueryRegistry {
public:
	QueryRegistry(string name) { value_ = GetRegistry().Register(name); }

	IdType value_;
	static RegistryMap& GetRegistry();
	static void SetRegistry(RegistryMap& registry);
	static RegistryMap* override_registry_;
};

class CommandRegistry {
public:
	CommandRegistry(string name) { value_ = GetRegistry().Register(name); }

	IdType value_;
	static RegistryMap& GetRegistry();
	static void SetRegistry(RegistryMap& registry);
	static RegistryMap* override_registry_;
};

}  // game_scene