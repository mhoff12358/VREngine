#include "stdafx.h"
#include "InteractableQuad.h"


InteractableQuad::InteractableQuad(
	const Identifier& id,
	Actor* actor,
	const array<float, 3>& point1,
	const array<float, 3>& point2,
	const array<float, 3>& point3) : LookInteractable(id, actor)
{
	DirectX::XMFLOAT4 base_point;
	base_point.x = point1[0];
	base_point.y = point1[1];
	base_point.z = point1[2];
	base_point.w = 1.0f;
	DirectX::XMFLOAT4 side_point_1;
	side_point_1.x = point2[0];
	side_point_1.y = point2[1];
	side_point_1.z = point2[2];
	side_point_1.w = 1.0f;
	DirectX::XMFLOAT4 side_point_2;
	side_point_2.x = point3[0];
	side_point_2.y = point3[1];
	side_point_2.z = point3[2];
	side_point_2.w = 1.0f;
	base_point_ = DirectX::XMLoadFloat4(&base_point);
	side_point_1_ = DirectX::XMLoadFloat4(&side_point_1);
	side_point_2_ = DirectX::XMLoadFloat4(&side_point_2);
}


InteractableQuad::~InteractableQuad()
{
}

float XM_CALLCONV InteractableQuad::IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMFLOAT4 base_point_trans;
	DirectX::XMFLOAT4 side_1_trans;
	DirectX::XMFLOAT4 side_2_trans;
	DirectX::XMStoreFloat4(&base_point_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(base_point_, model_transformation), view_transformation));
	DirectX::XMStoreFloat4(&side_1_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(side_point_1_, model_transformation), view_transformation));
	DirectX::XMStoreFloat4(&side_2_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(side_point_2_, model_transformation), view_transformation));
	
	side_1_trans.x -= base_point_trans.x;
	side_1_trans.y -= base_point_trans.y;
	side_1_trans.z -= base_point_trans.z;
	side_2_trans.x -= base_point_trans.x;
	side_2_trans.y -= base_point_trans.y;
	side_2_trans.z -= base_point_trans.z;

	// Trying to find where the plane containing the Quad intersects the z-axis
	// This point will be base_point_trans + x1*side_1_trans + x2*side_2_trans = <0, 0, -z>
	// Solving for the first two dimensions gives a system of 2 equations, which is a 2x2 matrix equation.
	float determinant = 1.0f / (side_1_trans.x * side_2_trans.y - side_1_trans.y * side_2_trans.x);
	float side_1_scale = (-base_point_trans.x*side_2_trans.y + base_point_trans.y*side_2_trans.x) * determinant;
	float side_2_scale = (base_point_trans.x*side_1_trans.y - base_point_trans.y*side_1_trans.x) * determinant;

	// If the z-axis intercept is more than one combined side-length away from the base point, or is
	// negative side lengths away, then it is not inside the Quad so return a default value.
	if (side_1_scale < 0 || side_2_scale < 0 || side_1_scale > 1 || side_2_scale > 1) {
		return std::nanf("");
	}

	return -(side_1_scale * side_1_trans.z + side_2_scale * side_2_trans.z + base_point_trans.z);
}

tuple<float, std::array<float, 2>> XM_CALLCONV InteractableQuad::WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMFLOAT4 base_point_trans;
	DirectX::XMFLOAT4 side_1_trans;
	DirectX::XMFLOAT4 side_2_trans;
	DirectX::XMStoreFloat4(&base_point_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(base_point_, model_transformation), view_transformation));
	DirectX::XMStoreFloat4(&side_1_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(side_point_1_, model_transformation), view_transformation));
	DirectX::XMStoreFloat4(&side_2_trans, DirectX::XMVector4Transform(DirectX::XMVector4Transform(side_point_2_, model_transformation), view_transformation));

	side_1_trans.x -= base_point_trans.x;
	side_1_trans.y -= base_point_trans.y;
	side_1_trans.z -= base_point_trans.z;
	side_2_trans.x -= base_point_trans.x;
	side_2_trans.y -= base_point_trans.y;
	side_2_trans.z -= base_point_trans.z;

	// Trying to find where the plane containing the triangle intersects the z-axis
	// This point will be base_point_trans + x1*side_1_trans + x2*side_2_trans = <0, 0, -z>
	// Solving for the first two dimensions gives a system of 2 equations, which is a 2x2 matrix equation.
	float determinant = 1.0f / (side_1_trans.x * side_2_trans.y - side_1_trans.y * side_2_trans.x);
	float side_1_scale = (-base_point_trans.x*side_2_trans.y + base_point_trans.y*side_2_trans.x) * determinant;
	float side_2_scale = (base_point_trans.x*side_1_trans.y - base_point_trans.y*side_1_trans.x) * determinant;

	// If the z-axis intercept is more than one combined side-length away from the base point, or is
	// negative side lengths away, then it is not inside the triangle so return a default value.
	if (side_1_scale < 0 || side_2_scale < 0 || side_1_scale > 1 || side_2_scale > 1) {
		return std::make_tuple(std::nanf(""), array<float, 2>({ { 0.0f, 0.0f } }));
	}

	return std::make_tuple(-(side_1_scale * side_1_trans.z + side_2_scale * side_2_trans.z + base_point_trans.z), array<float, 2>({ { side_1_scale, side_2_scale } }));
}