#include "Camera.h"

Camera::Camera() {
	location = { { 0.0f, 0.0f, 0.0f } };
	orientaiton = { { 0.0f, 0.0f, 0.0f, 0.0f } };
}

void Camera::InvalidateAllMatrices() {
	BuildLocationMatrix();
	BuildOrientationMatrix();
	BuildProjectionMatrix();

	BuildViewMatrix();
	BuildViewProjectionMatrix();
	BuildOrientationProjectionMatrix();
}

void Camera::InvalidateViewMatrices() {
	BuildLocationMatrix();
	BuildOrientationMatrix();

	BuildViewMatrix();
}

void Camera::BuildLocationMatrix() {
	DirectX::XMStoreFloat4x4(&location_matrix, DirectX::XMMatrixTranslation(
		-location[0],
		-location[1],
		-location[2]));
}

void Camera::BuildOrientationMatrix() {
	DirectX::XMStoreFloat4x4(&orientation_matrix, DirectX::XMMatrixRotationQuaternion(
		DirectX::XMVectorSet(-orientaiton[0], -orientaiton[1], -orientaiton[2], orientaiton[3])));
}

void Camera::BuildProjectionMatrix() {
	DirectX::XMStoreFloat4x4(&projection_matrix, DirectX::XMMatrixIdentity());
}

void Camera::BuildViewMatrix() {
	DirectX::XMStoreFloat4x4(&view_matrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&location_matrix), DirectX::XMLoadFloat4x4(&orientation_matrix)));
}

void Camera::BuildViewProjectionMatrix() {
	DirectX::XMStoreFloat4x4(&view_projection_matrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&view_matrix), DirectX::XMLoadFloat4x4(&projection_matrix)));
}

void Camera::BuildOrientationProjectionMatrix() {
	DirectX::XMStoreFloat4x4(&orientation_projection_matrix, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&orientation_matrix), DirectX::XMLoadFloat4x4(&projection_matrix)));
}

DirectX::XMMATRIX Camera::GetViewMatrix() {
	return DirectX::XMLoadFloat4x4(&view_matrix);
}

DirectX::XMMATRIX Camera::GetViewProjectionMatrix() {
	return DirectX::XMLoadFloat4x4(&view_projection_matrix);
}

DirectX::XMMATRIX Camera::GetOrientationProjectionMatrix() {
	return DirectX::XMLoadFloat4x4(&orientation_projection_matrix);
}
