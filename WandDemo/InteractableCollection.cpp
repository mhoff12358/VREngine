#include "stdafx.h"
#include "InteractableCollection.h"

#include <new>

LookInteractableBlock::LookInteractableBlock() {

}

LookInteractableBlock::~LookInteractableBlock() {
}

LookInteractable* LookInteractableBlock::GetAsLookInteractable() {
	return (LookInteractable*)(data);
}

const LookInteractable* LookInteractableBlock::GetAsLookInteractable() const {
	return (const LookInteractable*)(data);
}

InteractableCollection::InteractableCollection(unsigned int num_interactable_objects)
{
	interactable_objects_.reserve(num_interactable_objects);
}

InteractableCollection::~InteractableCollection()
{
}


tuple<Identifier, Actor*, float> XM_CALLCONV InteractableCollection::GetClosestLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	tuple<Identifier, Actor*, float> result_tuple;
	std::get<0>(result_tuple) = "";
	float& current_best = std::get<2>(result_tuple);
	current_best = std::numeric_limits<float>::infinity();
	for (const LookInteractableBlock& current_interactable_block : interactable_objects_) {
		const LookInteractable* current_interactable = current_interactable_block.GetAsLookInteractable();
		float current_looked_at_distance = current_interactable->IsLookedAt(view_transformation);
		if (current_looked_at_distance >= 0 && current_looked_at_distance < current_best) {
			current_best = current_looked_at_distance;
			std::get<0>(result_tuple) = current_interactable->GetId();
			std::get<1>(result_tuple) = current_interactable->GetActor();
		}
	}
	return result_tuple;
}

tuple<Identifier, Actor*, float, array<float, 2>> XM_CALLCONV InteractableCollection::GetClosestLookedAtAndWhere(const DirectX::FXMMATRIX& view_transformation) const {
	tuple<Identifier, Actor*, float, array<float, 2>> result_tuple;
	std::get<0>(result_tuple) = "";
	float& current_best = std::get<2>(result_tuple);
	current_best = std::numeric_limits<float>::infinity();
	for (const LookInteractableBlock& current_interactable_block : interactable_objects_) {
		const LookInteractable* current_interactable = current_interactable_block.GetAsLookInteractable();
		tuple<float, array<float, 2>> current_where_looked_at = current_interactable->WhereLookedAt(view_transformation);
		if (!std::isnan(std::get<0>(current_where_looked_at)) && std::get<0>(current_where_looked_at) >= 0 && std::get<0>(current_where_looked_at) < current_best) {
			current_best = std::get<0>(current_where_looked_at);
			std::get<0>(result_tuple) = current_interactable->GetId();
			std::get<1>(result_tuple) = current_interactable->GetActor();
			std::get<3>(result_tuple) = std::get<1>(current_where_looked_at);
		}
	}
	return result_tuple;
}

LookInteractable* InteractableCollection::GetInteractableAtPosition(int index) {
	if (index < 0 || index >= interactable_objects_.capacity()) {
		return NULL;
	}
	return interactable_objects_[index].GetAsLookInteractable();
}

LookInteractable* InteractableCollection::GetNewLookInteractableBlock() {
	interactable_objects_.push_back(LookInteractableBlock());
	return GetInteractableAtPosition(interactable_objects_.size() - 1);
}

