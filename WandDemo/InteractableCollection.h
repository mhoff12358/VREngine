#pragma once
#include "stdafx.h"

#include "Identifier.h"
#include "LookInteractable.h"

class InteractableCollection
{
public:
	InteractableCollection();
	~InteractableCollection();

	tuple<Identifier, float> XM_CALLCONV GetClosestLookedAt(const DirectX::FXMMATRIX& view_transformation) const;
	tuple<Identifier, float, array<float, 2>> XM_CALLCONV GetClosestLookedAtAndWhere(const DirectX::FXMMATRIX& view_transformation) const;

	void AddObject(LookInteractable* new_object);

private:
	vector<LookInteractable*> interactable_objects;
};

