#pragma once
#include "stdafx.h"

#include "Identifier.h"

#include <DirectXMath.h>

// An abstract class that represents and object that can detect if it is being
// looked directly at, and how far away it is from the camera.
class LookInteractable
{
public:
	LookInteractable(Identifier id);
	virtual ~LookInteractable();

	void XM_CALLCONV SetModelTransformation(const DirectX::FXMMATRIX& model_transformation);

	// This returns a value representing how far on the z-axis post view transformation the
	// object is. It doesn't return an exact distance, but the value it resturns has the same ordering
	// given other calls to this function wtih the same view_transformation.
	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const = 0;

	// This returns a tuple containing both the distance to the object and the location on the
	// object being looked at. The meaning of the location will depend on the class of the object.
	virtual tuple<float, std::array<float, 2>> XM_CALLCONV WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const = 0;

	const Identifier& GetId();

protected:
	Identifier id_;
	DirectX::XMMATRIX model_transformation_;
};

