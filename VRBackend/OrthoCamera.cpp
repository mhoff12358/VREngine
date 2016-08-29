#include "OrthoCamera.h"

OrthoCamera::OrthoCamera() : Camera() {
}

void OrthoCamera::BuildProjectionMatrix() {
	DirectX::XMStoreFloat4x4(&projection_matrix, DirectX::XMMatrixOrthographicRH(
		screen_size[0],
		screen_size[1],
		0.01f,
		10.0f));
}
