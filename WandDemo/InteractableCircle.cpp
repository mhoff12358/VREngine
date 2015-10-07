#include "stdafx.h"
#include "InteractableCircle.h"


InteractableCircle::InteractableCircle(
	const array<float, 3>& center,
	const array<float, 3>& normal,
	float radius) : radius_(radius)
{
	DirectX::XMFLOAT4 temp_center;
	temp_center.x = center[0];
	temp_center.y = center[1];
	temp_center.z = center[2];
	temp_center.w = 1.0f;
	DirectX::XMFLOAT4 temp_normal_point;
	temp_normal_point.x = center[0] + normal[0];
	temp_normal_point.y = center[1] + normal[1];
	temp_normal_point.z = center[2] + normal[2];
	temp_normal_point.w = 1.0f;
	center_ = DirectX::XMLoadFloat4(&temp_center);
	normal_point_ = DirectX::XMLoadFloat4(&temp_normal_point);
}


InteractableCircle::~InteractableCircle()
{
}

float XM_CALLCONV InteractableCircle::IsLookedAt(const DirectX::FXMMATRIX& view_transformation) {
	DirectX::XMFLOAT4 center_trans;
	DirectX::XMFLOAT4 normal_trans;
	DirectX::XMStoreFloat4(&center_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(center_, model_transformation_), view_transformation));
	DirectX::XMStoreFloat4(&normal_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(normal_point_, model_transformation_), view_transformation));
	
	normal_trans.x -= center_trans.x;
	normal_trans.y -= center_trans.y;
	normal_trans.z -= center_trans.z;

	float distance = (normal_trans.x * center_trans.x + normal_trans.y * center_trans.y + normal_trans.z * center_trans.z) / normal_trans.z;

	return -distance;
}
