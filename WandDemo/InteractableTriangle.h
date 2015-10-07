#pragma once
#include "stdafx.h"

#include "LookInteractable.h"

class InteractableTriangle :
	public LookInteractable
{
public:
	InteractableTriangle(
		const array<float, 3>& point_1,
		const array<float, 3>& point_2,
		const array<float, 3>& point_3);
	~InteractableTriangle();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) override;

private:
	DirectX::XMVECTOR base_point_;
	DirectX::XMVECTOR side_point_1_;
	DirectX::XMVECTOR side_point_2_;
};
