#pragma once
#include "stdafx.h"

#include "LookInteractable.h"

class InteractableCircle : public LookInteractable
{
public:
	InteractableCircle(
		const Identifier& id,
		float radius,
		const array<float, 3>& center,
		const array<float, 3>& normal,
		const array<float, 3>& right = { { 0, 0, 0 } });
	~InteractableCircle();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;
	virtual tuple<float, std::array<float, 2>> XM_CALLCONV WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;

private:
	DirectX::XMVECTOR center_;
	DirectX::XMVECTOR normal_point_;
	DirectX::XMVECTOR right_point_;
	float radius_;
};

