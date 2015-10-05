#include "stdafx.h"
#include "Actor.h"


Actor::Actor(ResourcePool& resource_pool)
	: resource_pool_(resource_pool), models_(), components_()
{
}


Actor::~Actor()
{
}

void Actor::LoadModelsFromFile(string file_name, const ObjLoader::OutputFormat& output_format) {
	models_ = resource_pool_.LoadModelAsParts(file_name, output_format);
}

map<string, unsigned int> Actor::CreateComponents(EntityHandler& entity_handler, ID3D11Device* device_interface, const multimap<string, vector<string>>& parentages) {
	int current_parent_index = -1;
	int current_children_index = 0;
	int current_num_children = 0;
	vector<string> component_names;
	map<string, unsigned int> component_locations;
	components_.reserve(parentages.size());

	// This while loop check should be redundant as both of these conditions should become false at the same time.
	// However, checking both ensures that components_ isn't underfilled for what is referenced and that
	// it doesn't grow infinitely.
	while ((current_parent_index != components_.size()) && (components_.size() <= models_.size())) {
		string next_parent = "";
		for (const pair<string, vector<string>>& parentage : parentages) {
			if ((current_parent_index != -1 && parentage.first == component_names[current_parent_index]) || (current_parent_index == -1 && parentage.first == "")) {
				vector<Model> current_children;
				for (const string& child_model_name : parentage.second) {
					auto child_model = models_.find(child_model_name);
					assert(child_model != models_.end());
					current_children.push_back(child_model->second);
				}
				components_.emplace_back(entity_handler, device_interface, current_children);
				components_.back().SetLocalTransformation(DirectX::XMMatrixIdentity());
				component_names.push_back(parentage.second.front());
				component_locations[parentage.second.front()] = components_.size() - 1;
				std::cerr << "Adding component with parent: " << (current_parent_index == -1 ? "" : component_names[current_parent_index]) << " and name: " << parentage.second.front() << std::endl;
				current_num_children++;
			}
		}

		if (current_parent_index != -1) {
			components_[current_parent_index].SetChildren(components_.data() + current_children_index, current_num_children);
		}

		current_children_index += current_num_children;
		current_num_children = 0;
		current_parent_index++;
	}

	return component_locations;
}

void XM_CALLCONV Actor::SetComponentTransformation(unsigned int component_index, DirectX::FXMMATRIX new_transformation) {
	components_[component_index].SetLocalTransformation(new_transformation);
}
