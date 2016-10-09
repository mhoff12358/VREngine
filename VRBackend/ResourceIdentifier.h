#pragma once

#include "stl.h"

class ResourceIdentifier {
public:
	static string object_delimiter;
	static string unique_identifier;
	static int unique_number;

	static string StripSubmodel(string name) {
		return name.substr(0, name.find(object_delimiter));
	};
	static string GetSubmodel(string name) {
		return name.substr(max(name.find(object_delimiter) + 1, name.length()));
	}
	static string AddSubmodel(string name, string submodel) {
		return name + object_delimiter + submodel;
	}
	static string GetNewModelName(string description = "") {
		return unique_identifier + description + unique_identifier + std::to_string(unique_number++);
	}
	static string GetConstantModelName(string description) {
		return unique_identifier + description;
	}
};

class ModelIdentifier {
public:
	ModelIdentifier() {}
	explicit ModelIdentifier(string file_name)
		: file_name_(ResourceIdentifier::StripSubmodel(file_name)),
		  sub_part_(ResourceIdentifier::GetSubmodel(file_name)) {}
	ModelIdentifier(string file_name, string sub_part) : file_name_(move(file_name)), sub_part_(move(sub_part)) {}

	const string& GetFileName() const {
		return file_name_;
	}

	const string& GetSubPart() const {
		return sub_part_;
	}

	string GetAsString() const {
		return ResourceIdentifier::AddSubmodel(file_name_, sub_part_);
	}

	bool operator==(const ModelIdentifier& other) const {
		return file_name_ == other.file_name_ && sub_part_ == other.sub_part_;
	}

	bool operator<(const ModelIdentifier& other) const {
		if (file_name_ < other.file_name_) {
			return true;
		}
		else if (other.file_name_ < file_name_) {
			return false;
		} else {
			return sub_part_ < other.sub_part_;
		}
	}

private:
	string file_name_;
	string sub_part_;
};