#pragma once
#include "stdafx.h"

#include "LookInteractable.h"

class InteractableSphere : public LookInteractable
{
public:
	InteractableSphere(
		const Identifier& id,
		Actor* actor,
		float radius,
		const array<float, 3>& center);
	~InteractableSphere();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;
	virtual tuple<float, std::array<float, 2>> XM_CALLCONV WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;

private:
	DirectX::XMVECTOR center_;
	float radius_;
};

