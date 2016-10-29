#pragma once

#include "stdafx.h"

#include "VRBackend/ConstantBuffer.h"
#include "VRBackend/Model.h"

class EntityHandler;

class Component
{
public:
	explicit Component(ID3D11Device* device_interface);
	Component();
	~Component();

	Component(const Component& other) = delete;
	Component operator=(const Component& other) = delete;
	Component(Component&& other);

	void SetChildren(Component* children, int num_children);

	void AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model> models);
	void AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model>::iterator first_model, vector<Model>::iterator end_of_models);

	void XM_CALLCONV SetLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV LeftComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV RightComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void UpdateTransformation();
	const TransformationMatrixAndInvTransData* GetTransformationData();
	ConstantBuffer* GetTransformationBuffer();
	const DirectX::XMMATRIX& GetLocalTransformation();
	const DirectX::XMMATRIX& GetCombinedTransformation();

	unsigned int first_entity_;
	unsigned int number_of_entities_;

private:
	void SetParent(Component* parent);

	Component* children_;
	unsigned int num_children_;

	ConstantBufferTyped<TransformationMatrixAndInvTransData> transformation_buffer_;

	DirectX::XMMATRIX local_transformation_;
	DirectX::XMMATRIX combined_transformation_;
	DirectX::XMMATRIX* parent_transformation_;
};

namespace game_scene {

struct ComponentInfo {
	ComponentInfo(string parent_name, string name)
		: parent_name_(parent_name), name_(name) {}

	// These accesses are only needed internally.
	void AddChild(const ComponentInfo& new_child) {
		children_.push_back(&new_child);
	}

	string parent_name_;
	string name_;

	vector<const ComponentInfo*> children_;
};

struct ComponentHeirarchyBuilder {
	ComponentHeirarchyBuilder(ID3D11Device* device_interface) : device_interface_(device_interface) {}

	ComponentHeirarchyBuilder(const ComponentHeirarchyBuilder& other) = delete;
	ComponentHeirarchyBuilder operator=(const ComponentHeirarchyBuilder& other) = delete;

	void BuildComponents(vector<ComponentInfo> components, vector<Component>* components_output, map<string, size_t>* component_lookup_output) {
		// Configure the parents to own their children and build a list of all roots.
		map<string, ComponentInfo*> component_lookup_by_name;
		vector<ComponentInfo*> root_components;
		for (ComponentInfo& component : components) {
			if (component_lookup_by_name.count(component.name_) != 0) {
				std::cout << "Duplicately named component " << component.name_ << std::endl;
			}
			component_lookup_by_name[component.name_] = &component;
			if (component.parent_name_.empty()) {
				root_components.push_back(&component);
			} else {
				auto parent_iter = component_lookup_by_name.find(component.parent_name_);
				if (parent_iter == component_lookup_by_name.end()) {
					std::cout << "Misconfiguration in component heirarchy" << std::endl;
				}
				parent_iter->second->AddChild(component);
			}
		}

		// Pre-create all the components.
		components_.reserve(components.size());

		for (const ComponentInfo* root_component : root_components) {
			// Add the root component and handle its children.
			components_.emplace_back(device_interface_);
			AddDetailedComponentInfo(*root_component, components_.size() - 1);
		}

		*components_output = move(components_);
		*component_lookup_output = move(component_lookup_);
	}

	void AddDetailedComponentInfo(const ComponentInfo& info, size_t own_index) {
		component_lookup_[info.name_] = own_index;
		if (!info.children_.empty()) {
			// Packs all of info's children onto the end of the buffer.
			size_t children_index = components_.size();
			for (size_t i = 0; i < info.children_.size(); i++) {
				components_.emplace_back(device_interface_);
			}
			// Sets info's children now that they are placed.
			components_[own_index].SetChildren(&components_[children_index], info.children_.size());
			// Has each child update its own children.
			for (const ComponentInfo* child : info.children_) {
				AddDetailedComponentInfo(*child, children_index);
				children_index += 1;
			}
		}
	}

	vector<Component> components_;
	map<string, size_t> component_lookup_;
	ID3D11Device* device_interface_;
};

}  // game_scene
