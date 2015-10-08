#include "stdafx.h"
#include "InteractableCollection.h"


InteractableCollection::InteractableCollection()
{
}


InteractableCollection::~InteractableCollection()
{
}


tuple<Identifier, float> XM_CALLCONV InteractableCollection::GetClosestLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	tuple<Identifier, float> result_tuple;
	std::get<0>(result_tuple) = "";
	float& current_best = std::get<1>(result_tuple);
	current_best = std::numeric_limits<float>::infinity();
	for (LookInteractable* current_interactable : interactable_objects) {
		float current_looked_at_distance = current_interactable->IsLookedAt(view_transformation);
		if (current_looked_at_distance >= 0 && current_looked_at_distance < current_best) {
			current_best = current_looked_at_distance;
			std::get<0>(result_tuple) = current_interactable->GetId();
		}
	}
	return result_tuple;
}

tuple<Identifier, float, array<float, 2>> XM_CALLCONV InteractableCollection::GetClosestLookedAtAndWhere(const DirectX::FXMMATRIX& view_transformation) const {
	tuple<Identifier, float, array<float, 2>> result_tuple;
	std::get<0>(result_tuple) = "";
	float& current_best = std::get<1>(result_tuple);
	current_best = std::numeric_limits<float>::infinity();
	for (LookInteractable* current_interactable : interactable_objects) {
		tuple<float, array<float, 2>> current_where_looked_at = current_interactable->WhereLookedAt(view_transformation);
		if (!std::isnan(std::get<0>(current_where_looked_at)) && std::get<0>(current_where_looked_at) >= 0 && std::get<0>(current_where_looked_at) < current_best) {
			current_best = std::get<0>(current_where_looked_at);
			std::get<0>(result_tuple) = current_interactable->GetId();
			std::get<2>(result_tuple) = std::get<1>(current_where_looked_at);
		}
	}
	return result_tuple;
}

void InteractableCollection::AddObject(LookInteractable* new_object) {
	interactable_objects.push_back(new_object);
}
