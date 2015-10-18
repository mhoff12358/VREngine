#pragma once
#include "stdafx.h"

#include "Identifier.h"
#include "LookInteractable.h"
#include "InteractableCircle.h"
#include "InteractableTriangle.h"
#include "InteractableQuad.h"

#include "LuaGameScripting/Environment.h"

#define CONSTMAX(a, b) (a > b ? a : b)

struct LookInteractableBlock {
	LookInteractableBlock();
	~LookInteractableBlock();

	char data[CONSTMAX(sizeof(InteractableTriangle), CONSTMAX(sizeof(InteractableQuad), sizeof(InteractableCircle)))];

	LookInteractable* GetAsLookInteractable();
	const LookInteractable* GetAsLookInteractable() const;
};

class InteractableCollection
{
public:
	InteractableCollection(unsigned int num_interactable_objects);
	~InteractableCollection();

	tuple<Identifier, float> XM_CALLCONV GetClosestLookedAt(const DirectX::FXMMATRIX& view_transformation) const;
	tuple<Identifier, float, array<float, 2>> XM_CALLCONV GetClosestLookedAtAndWhere(const DirectX::FXMMATRIX& view_transformation) const;

	LookInteractable* GetInteractableAtPosition(int index);
	LookInteractable* GetNewLookInteractableBlock();
	LookInteractable* CreateLookInteractableFromLua(Lua::Environment environment_with_table);
	
private:
	vector<LookInteractableBlock> interactable_objects_;
};
