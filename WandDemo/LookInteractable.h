#pragma once
#include <DirectXMath.h>

// An abstract class that represents and object that can detect if it is being
// looked directly at, and how far away it is from the camera.
class LookInteractable
{
public:
	LookInteractable();
	virtual ~LookInteractable();

	void XM_CALLCONV SetModelTransformation(const DirectX::FXMMATRIX& model_transformation);

	// This returns a value representing how far on the z-axis post view transformation the
	// object is. It doesn't return an exact distance, but the value it resturns has the same ordering
	// given other calls to this function wtih the same view_transformation.
	virtual float XM_CALLCONV IsLookedAt(const DirectX::FXMMATRIX& view_transformation) = 0;

protected:
	DirectX::XMMATRIX model_transformation_;
};

