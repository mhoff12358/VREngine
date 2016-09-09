#pragma once
#include "stdafx.h"

#define REGISTER_COMMAND(class_name, command_name) CommandRegistry class_name::REGISTRY_##command_name(#class_name":"#command_name)
#define DECLARE_COMMAND(class_name, command_name) static CommandRegistry REGISTRY_##command_name; static constexpr ::game_scene::IdType command_name = FNV(#class_name":"#command_name);
#define REGISTER_QUERY(class_name, query_name) QueryRegistry class_name::REGISTRY_##query_name(#class_name":"#query_name)
#define DECLARE_QUERY(class_name, query_name) static QueryRegistry REGISTRY_##query_name; static constexpr ::game_scene::IdType query_name = FNV(#class_name":"#query_name);

namespace game_scene {

typedef int64_t IdType;

constexpr IdType FNV(const char* string) {
#pragma warning (push)
#pragma warning (disable : CS4307)
	return *string == '\0' ? 14695981039346656037 : 1099511628211 * (*string ^ FNV(string + 1));
#pragma warning (pop)
}

class RegistryMap {
public:
	IdType Register(string new_value) {
		GetRegistries().push_back(new_value);
		const auto& mapping = GetMapping();
		IdType value = FNV(new_value.data());
		if (mapping.count(value) != 0) {
			std::cout << "Hash collision occured in registry" << std::endl;
		}
		GetMapping().insert(make_pair(value, new_value));
		return value;
	}

	IdType IdFromName(const string& name) {
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
	QueryRegistry(string name) { value_ = mapping_.Register(name); }

	IdType value_;

	static RegistryMap mapping_;
};

class CommandRegistry {
public:
	CommandRegistry(string name) { value_ = mapping_.Register(name); }

	IdType value_;

	static RegistryMap mapping_;
};

}  // game_scene