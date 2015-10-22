#include "stdafx.h"
#include "InteractableSphere.h"


InteractableSphere::InteractableSphere(
	const Identifier& id,
	Actor* actor,
	float radius,
	const array<float, 3>& center) : LookInteractable(id, actor), radius_(radius)
{
	DirectX::XMFLOAT4 temp_center;
	temp_center.x = center[0];
	temp_center.y = center[1];
	temp_center.z = center[2];
	temp_center.w = 1.0f;
	center_ = DirectX::XMLoadFloat4(&temp_center);
}


InteractableSphere::~InteractableSphere()
{
}

float XM_CALLCONV InteractableSphere::IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMFLOAT4 center_trans;
	DirectX::XMVECTOR center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&center_trans, center_trans_vec);

	// Finding where a sphere of radius radius_ intersects the z-axis.
	// The distance from a point (0, 0, z) to the center of the sphere is c_z - sqrt(r^2 - c_x^2 - c_y^2)
	// If the sqrt doesn't exist, then there is no intersection.

	float distance_left_for_z_axis = radius_ * radius_ - center_trans.x * center_trans.x - center_trans.y * center_trans.y;
	if (distance_left_for_z_axis < 0) {
		return std::nanf("");
	}

	return center_trans.z - std::powf(distance_left_for_z_axis, 0.5);
}

tuple<float, std::array<float, 2>> XM_CALLCONV InteractableSphere::WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMFLOAT4 center_trans;
	DirectX::XMVECTOR center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&center_trans, center_trans_vec);

	// Finding where a sphere of radius radius_ intersects the z-axis.
	// The distance from a point (0, 0, z) to the center of the sphere is c_z - sqrt(r^2 - c_x^2 - c_y^2)
	// If the sqrt doesn't exist, then there is no intersection.

	float distance_left_for_z_axis = radius_ * radius_ - center_trans.x * center_trans.x - center_trans.y * center_trans.y;
	if (distance_left_for_z_axis < 0) {
		return std::make_tuple(std::nanf(""), array<float, 2>({ { 0.0f, 0.0f } }));
	}
	
	float z_intercept = center_trans.z + std::powf(distance_left_for_z_axis, 0.5);

	// Need to find the theta and phi from (c_x, c_y, c_z) to (0, 0, z_intercept)

	float phi = std::asinf(-center_trans.y);
	float theta = std::atan2f(z_intercept - center_trans.z, -center_trans.x);

	return std::make_tuple(-z_intercept, array<float, 2>({ theta, phi }));
}
