#pragma once

#include "DirectXMath.h"
#include "Headset.h"
#include "Pose.h"
#include "stl.h"

typedef string PipelineCameraIdent;

class PipelineCamera {
public:
	void BuildMatrices();

	const DirectX::XMMATRIX& GetViewProjectionMatrix() const;
	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetOrientationProjectionMatrix() const;

	void SetOrthoProjection(float width, float height, float near_z, float far_z);
	void SetPerspectiveProjection(float vertical_fov, float aspect_ratio, float near_z, float far_z);
	void SetRawProjection(DirectX::XMMATRIX projection_matrix);
	//void SetPerspectiveProjection(ovrFovPort oculus_fov, float near_z, float far_z);
	void SetLocation(array<float, 3> location);
	void SetLocation(const Location& location);
	void SetOrientation(array<float, 4> orientation);
	void SetOrientation(const Quaternion& orientation);

private:
	// Base component matrices defined by input.
	DirectX::XMMATRIX location_matrix_;
	DirectX::XMMATRIX orientation_matrix_;
	DirectX::XMMATRIX projection_matrix_;

	// Calculated matrices
	DirectX::XMMATRIX view_matrix_;
	DirectX::XMMATRIX view_projection_matrix_;
	DirectX::XMMATRIX orientation_projection_matrix_;
};
