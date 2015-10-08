#pragma once
#include "stdafx.h"

#include "LookInteractable.h"

class InteractableQuad :
	public LookInteractable
{
public:
	InteractableQuad(
		const Identifier& id,
		const array<float, 3>& point_1,
		const array<float, 3>& point_2,
		const array<float, 3>& point_3);
	~InteractableQuad();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;
	virtual tuple<float, std::array<float, 2>> XM_CALLCONV WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;

private:
	DirectX::XMVECTOR base_point_;
	DirectX::XMVECTOR side_point_1_;
	DirectX::XMVECTOR side_point_2_;
};
