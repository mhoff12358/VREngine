#pragma once
#include "stdafx.h"

#include "Identifier.h"
#include "LookInteractable.h"
#include "InteractableCircle.h"
#include "InteractableTriangle.h"
#include "InteractableQuad.h"

#include "LuaGameScripting/Environment.h"

class InteractableCollection
{
public:
	InteractableCollection();
	~InteractableCollection();

	tuple<Identifier, float> XM_CALLCONV GetClosestLookedAt(const DirectX::FXMMATRIX& view_transformation) const;
	tuple<Identifier, float, array<float, 2>> XM_CALLCONV GetClosestLookedAtAndWhere(const DirectX::FXMMATRIX& view_transformation) const;

	void AddObject(LookInteractable* new_object);

	static LookInteractable* CreateLookInteractableFromLua(Lua::Environment environment_with_table);
	LookInteractable* CreateAndAddLookInteractableFromLua(Lua::Environment environment_with_table);

private:
	vector<LookInteractable*> interactable_objects;
};

