#pragma once

#include "stdafx.h"

#include "LookInteractable.h"

class InteractableCircle : public LookInteractable
{
public:
	InteractableCircle(
		const array<float, 3>& center,
		const array<float, 3>& normal,
		float radius);
	~InteractableCircle();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) override;

private:
	DirectX::XMVECTOR center_;
	DirectX::XMVECTOR normal_point_;
	float radius_;
};

