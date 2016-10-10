#include "PipelineCamera.h"

void PipelineCamera::SetOrthoProjection(float width, float height, float near_z, float far_z) {
	projection_matrix_ = DirectX::XMMatrixOrthographicRH(
		width,
		height,
		near_z,
		far_z);
	SetDirty();
}

void PipelineCamera::SetPerspectiveProjection(float vertical_fov, float aspect_ratio, float near_z, float far_z) {
	projection_matrix_ = DirectX::XMMatrixPerspectiveFovRH(
		vertical_fov,
		aspect_ratio,
		near_z,
		far_z);
	SetDirty();
}

void PipelineCamera::SetRawProjection(DirectX::XMMATRIX projection_matrix) {
	projection_matrix_ = projection_matrix;
	SetDirty();
}

void PipelineCamera::SetLocation(array<float, 3> location) {
	location_matrix_ = DirectX::XMMatrixTranslation(-location[0], -location[1], -location[2]);
	SetDirty();
}

void PipelineCamera::SetLocation(const Location& location) {
	SetLocation(location.location_);
}

void PipelineCamera::SetOrientation(array<float, 4> orientation) {
	orientation_matrix_ = DirectX::XMMatrixRotationQuaternion(
		DirectX::XMVectorSet(-orientation[0], -orientation[1], -orientation[2], orientation[3]));
	SetDirty();
}

void PipelineCamera::SetOrientation(const Quaternion& orientation) {
	SetOrientation(orientation.GetArray());
}

void PipelineCamera::SetPose(const Pose& pose) {
	SetLocation(pose.location_);
	SetOrientation(pose.orientation_);
}

void PipelineCamera::BuildMatrices() {
	orientation_projection_matrix_ = orientation_matrix_ * projection_matrix_;
	view_matrix_ = location_matrix_ * orientation_matrix_;
	view_projection_matrix_ = view_matrix_ * projection_matrix_;
}

void PipelineCamera::BuildMatricesIfDirty() {
	if (dirty_) {
		BuildMatrices();
		dirty_ = false;
	}
}

void PipelineCamera::SetDirty() {
	dirty_ = true;
}

const DirectX::XMMATRIX& PipelineCamera::GetViewMatrix() const {
	return view_matrix_;
}

const DirectX::XMMATRIX& PipelineCamera::GetViewProjectionMatrix() const {
	return view_projection_matrix_;
}

const DirectX::XMMATRIX& PipelineCamera::GetOrientationProjectionMatrix() const {
	return orientation_projection_matrix_;
}