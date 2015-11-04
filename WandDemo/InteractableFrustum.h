#pragma once
#include "LookInteractable.h"
class InteractableFrustum :
	public LookInteractable
{
public:
	InteractableFrustum(
		const Identifier& id,
		Actor* actor,
		float base_radius,
		float top_radius,
		const array<float, 3>& base_center,
		const array<float, 3>& top_center,
		const array<float, 3>& right_radius,
		bool base_included = false,
		bool top_included = false);
	~InteractableFrustum();

	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;
	virtual tuple<float, std::array<float, 2>> XM_CALLCONV WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const override;

private:
	DirectX::XMVECTOR base_center_;
	DirectX::XMVECTOR top_center_;
	DirectX::XMVECTOR right_radius_;
	DirectX::XMVECTOR forward_radius_;
	float base_radius_;
	float top_radius_;
	bool base_included_;
	bool top_included_;

	static tuple<bool, bool, bool, bool> GetIntercepts(pair<float, float> p1, pair<float, float> p2);
	static void CombineIntercepts(tuple<bool, bool, bool, bool>& existing_intercepts, tuple<bool, bool, bool, bool> new_intercepts);
};

