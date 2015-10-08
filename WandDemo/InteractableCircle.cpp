#include "stdafx.h"
#include "InteractableCircle.h"


InteractableCircle::InteractableCircle(
	const Identifier& id,
	float radius,
	const array<float, 3>& center,
	const array<float, 3>& normal,
	const array<float, 3>& right) : LookInteractable(id), radius_(radius)
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
	DirectX::XMFLOAT4 temp_right_point;
	temp_right_point.x = center[0] + right[0];
	temp_right_point.y = center[1] + right[1];
	temp_right_point.z = center[2] + right[2];
	temp_right_point.w = 1.0f;
	center_ = DirectX::XMLoadFloat4(&temp_center);
	normal_point_ = DirectX::XMLoadFloat4(&temp_normal_point);
	right_point_ = DirectX::XMLoadFloat4(&temp_right_point);
}


InteractableCircle::~InteractableCircle()
{
}

float XM_CALLCONV InteractableCircle::IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMFLOAT4 center_trans;
	DirectX::XMFLOAT4 normal_trans;
	DirectX::XMStoreFloat4(&center_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(center_, model_transformation_), view_transformation));
	DirectX::XMStoreFloat4(&normal_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(normal_point_, model_transformation_), view_transformation));
	
	normal_trans.x -= center_trans.x;
	normal_trans.y -= center_trans.y;
	normal_trans.z -= center_trans.z;

	float distance_from_viewer = (normal_trans.x * center_trans.x + normal_trans.y * center_trans.y + normal_trans.z * center_trans.z) / normal_trans.z;

	//std::cout << center_trans.x << ", " << center_trans.y << ", " << center_trans.z << std::endl;
	//std::cout << normal_trans.x << ", " << normal_trans.y << ", " << normal_trans.z << std::endl;
	float distance_from_center_squared = center_trans.x * center_trans.x + center_trans.y * center_trans.y + (center_trans.z - distance_from_viewer) * (center_trans.z - distance_from_viewer);
	if (distance_from_center_squared > radius_ * radius_) {
		return std::nanf("");
	}

	return -distance_from_viewer;
}

tuple<float, std::array<float, 2>> XM_CALLCONV InteractableCircle::WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMFLOAT4 center_trans;
	DirectX::XMFLOAT4 normal_trans;
	DirectX::XMVECTOR center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(center_, model_transformation_), view_transformation);
	DirectX::XMStoreFloat4(&center_trans, center_trans_vec);
	DirectX::XMStoreFloat4(&normal_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(normal_point_, model_transformation_), view_transformation));

	normal_trans.x -= center_trans.x;
	normal_trans.y -= center_trans.y;
	normal_trans.z -= center_trans.z;

	float distance_from_viewer = (normal_trans.x * center_trans.x + normal_trans.y * center_trans.y + normal_trans.z * center_trans.z) / normal_trans.z;

	//std::cout << center_trans.x << ", " << center_trans.y << ", " << center_trans.z << std::endl;
	//std::cout << normal_trans.x << ", " << normal_trans.y << ", " << normal_trans.z << std::endl;
	float distance_from_center_squared = center_trans.x * center_trans.x + center_trans.y * center_trans.y + (center_trans.z - distance_from_viewer) * (center_trans.z - distance_from_viewer);
	if (distance_from_center_squared > radius_ * radius_) {
		return std::make_tuple(std::nanf(""), array<float, 2>({ { 0.0f, 0.0f } }));
	}
	
	// Now that the point (0, 0, distance) has been found in the same plane as the circle, the angle
	// between the right vector and this one must be computed.

	DirectX::XMFLOAT4 right_trans;
	DirectX::XMStoreFloat4(&right_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(right_point_, model_transformation_), view_transformation));
	right_trans.x -= center_trans.x;
	right_trans.y -= center_trans.y;
	right_trans.z -= center_trans.z;
	DirectX::XMVECTOR right_trans_vec = DirectX::XMLoadFloat4(&right_trans);

	DirectX::XMFLOAT4 intersection_trans;
	intersection_trans.x = -center_trans.x;
	intersection_trans.y = -center_trans.y;
	intersection_trans.z = distance_from_viewer - center_trans.z;
	DirectX::XMVECTOR intersection_trans_vec = DirectX::XMLoadFloat4(&intersection_trans);

	DirectX::XMVECTOR normal_trans_vec = DirectX::XMLoadFloat4(&normal_trans);

	array<float, 2> look_location;
	DirectX::XMStoreFloat(look_location.data(), DirectX::XMVector3AngleBetweenVectors(right_trans_vec, intersection_trans_vec));
	DirectX::XMStoreFloat(look_location.data()+1, DirectX::XMVector3Length(intersection_trans_vec));
	look_location[1] /= radius_;

	float cross_normal_dot;
	DirectX::XMStoreFloat(&cross_normal_dot, DirectX::XMVector3Dot(DirectX::XMVector3Cross(right_trans_vec, intersection_trans_vec), normal_trans_vec));

	if (cross_normal_dot < 0) {
		look_location[0] = 2 * pi - look_location[0];
	}

	return std::make_tuple(-distance_from_viewer, look_location);
}
