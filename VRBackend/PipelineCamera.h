#pragma once

#include "stdafx.h"

typedef string PipelineCameraIdent;

class PipelineCamera {
public:
	void BuildMatrices();
	void BuildMatricesIfDirty();

	void SetDirty();

	const DirectX::XMMATRIX& GetViewProjectionMatrix() const;
	const DirectX::XMMATRIX& GetViewMatrix() const;
	const DirectX::XMMATRIX& GetViewInverseMatrix() const;
	const DirectX::XMMATRIX& GetOrientationProjectionMatrix() const;

	void SetOrthoProjection(float width, float height, float near_z, float far_z);
	void SetPerspectiveProjection(float vertical_fov, float aspect_ratio, float near_z, float far_z);
	void SetRawProjection(DirectX::XMMATRIX projection_matrix);
	void SetLocation(array<float, 3> location);
	void SetLocation(const Location& location);
	void SetOrientation(array<float, 4> orientation);
	void SetOrientation(const Quaternion& orientation);
	void SetPose(const Pose& pose);

private:
	// Base component matrices defined by input.
	DirectX::XMMATRIX location_matrix_;
	DirectX::XMMATRIX orientation_matrix_;
	DirectX::XMMATRIX projection_matrix_;
	DirectX::XMMATRIX location_inverse_matrix_;
	DirectX::XMMATRIX orientation_inverse_matrix_;

	// Calculated matrices
	DirectX::XMMATRIX view_matrix_;
	DirectX::XMMATRIX view_inverse_matrix_;
	DirectX::XMMATRIX view_projection_matrix_;
	DirectX::XMMATRIX orientation_projection_matrix_;

	bool dirty_ = false;
};
