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

LookInteractable* InteractableCollection::CreateLookInteractableFromLua(Lua::Environment environment_with_table) {
	string interactable_type;
	if (!environment_with_table.LoadFromTable(string("interactable_type"), &interactable_type)) {
		return NULL;
	}
	if (interactable_type == "Circle") {
		string id;
		float radius;
		array<float, 3> center;
		array<float, 3> normal;
		array<float, 3> right;
		if (!(environment_with_table.LoadFromTable(string("id"), &id) &&
			environment_with_table.LoadFromTable(string("radius"), &radius) &&
			environment_with_table.LoadArrayFromTable(string("center"), center.data(), Lua::Environment::stack_top, 3) &&
			environment_with_table.LoadArrayFromTable(string("normal"), normal.data(), Lua::Environment::stack_top, 3))) {
			return NULL;
		}
		if (environment_with_table.LoadArrayFromTable(string("right"), right.data(), Lua::Environment::stack_top, 3)) {
			return new InteractableCircle(id, radius, center, normal, right);
		} else {
			return new InteractableCircle(id, radius, center, normal);
		}
	}
	else if (interactable_type == "Quad") {
		string id;
		array<float, 3> point_1;
		array<float, 3> point_2;
		array<float, 3> point_3;
		if (!(environment_with_table.LoadFromTable(string("id"), &id) &&
			environment_with_table.LoadArrayFromTable(string("point_1"), point_1.data()) &&
			environment_with_table.LoadArrayFromTable(string("point_2"), point_2.data(), Lua::Environment::stack_top, 3) &&
			environment_with_table.LoadArrayFromTable(string("point_3"), point_3.data(), Lua::Environment::stack_top, 3))) {
			return NULL;
		}
		return new InteractableQuad(id, point_1, point_2, point_3);
	} else if (interactable_type == "Triangle") {
		string id;
		array<float, 3> point_1;
		array<float, 3> point_2;
		array<float, 3> point_3;
		if (!(environment_with_table.LoadFromTable(string("id"), &id) &&
			environment_with_table.LoadArrayFromTable(string("point_1"), point_1.data()) &&
			environment_with_table.LoadArrayFromTable(string("point_2"), point_2.data(), Lua::Environment::stack_top, 3) &&
			environment_with_table.LoadArrayFromTable(string("point_3"), point_3.data(), Lua::Environment::stack_top, 3))) {
			return NULL;
		}
		return new InteractableTriangle(id, point_1, point_2, point_3);
	} else {
		return NULL;
	}
}

LookInteractable* InteractableCollection::CreateAndAddLookInteractableFromLua(Lua::Environment environment_with_table) {
	LookInteractable* created_object = CreateLookInteractableFromLua(environment_with_table);
	if (created_object != NULL) {
		AddObject(created_object);
	}
	return created_object;
}