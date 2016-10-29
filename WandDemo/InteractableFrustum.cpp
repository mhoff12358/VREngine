#include "stdafx.h"
#include "InteractableFrustum.h"


InteractableFrustum::InteractableFrustum(
	const Identifier& id,
	Actor* actor,
	float base_radius,
	float top_radius,
	const array<float, 3>& base_center,
	const array<float, 3>& top_center,
	const array<float, 3>& right_radius,
	bool base_included,
	bool top_included)
	: LookInteractable(id, actor), base_radius_(base_radius), top_radius_(top_radius), base_included_(base_included), top_included_(top_included)
{
	DirectX::XMFLOAT4 temp_base_center;
	temp_base_center.x = base_center[0];
	temp_base_center.y = base_center[1];
	temp_base_center.z = base_center[2];
	temp_base_center.w = 1.0f;
	DirectX::XMFLOAT4 temp_top_center;
	temp_top_center.x = top_center[0];
	temp_top_center.y = top_center[1];
	temp_top_center.z = top_center[2];
	temp_top_center.w = 1.0f;
	DirectX::XMFLOAT3 temp_right_radius;
	temp_right_radius.x = right_radius[0];
	temp_right_radius.y = right_radius[1];
	temp_right_radius.z = right_radius[2];
	base_center_ = DirectX::XMLoadFloat4(&temp_base_center);
	top_center_ = DirectX::XMLoadFloat4(&temp_top_center);
	right_radius_ = DirectX::XMVectorSetW(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&temp_right_radius)), 0);
	forward_radius_ = DirectX::XMVectorSetW(DirectX::XMVector3Cross(DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(top_center_, base_center_)), right_radius_), 0);

	right_radius_ = DirectX::XMVectorAdd(base_center_, right_radius_);
	forward_radius_ = DirectX::XMVectorAdd(base_center_, forward_radius_);
}


InteractableFrustum::~InteractableFrustum()
{
}


float XM_CALLCONV InteractableFrustum::IsLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMVECTOR base_center_trans_vec;
	DirectX::XMVECTOR top_center_trans_vec;
	DirectX::XMFLOAT4 base_center_trans;
	DirectX::XMFLOAT4 top_center_trans;
	DirectX::XMFLOAT4 right_radius_trans;
	DirectX::XMFLOAT4 forward_radius_trans;
	base_center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(base_center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&base_center_trans, base_center_trans_vec);
	top_center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(top_center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&top_center_trans, top_center_trans_vec);
	DirectX::XMStoreFloat4(&right_radius_trans, DirectX::XMVectorSubtract(DirectX::XMVector4Transform(DirectX::XMVector4Transform(right_radius_, model_transformation), view_transformation), base_center_trans_vec));
	DirectX::XMStoreFloat4(&forward_radius_trans, DirectX::XMVectorSubtract(DirectX::XMVector4Transform(DirectX::XMVector4Transform(forward_radius_, model_transformation), view_transformation), base_center_trans_vec));

	float matrix_scaling = sqrtf(powf(right_radius_trans.x, 2) + powf(right_radius_trans.y, 2) + powf(right_radius_trans.z, 2));
	float scaled_base_radius = matrix_scaling * base_radius_;
	float scaled_top_radius = matrix_scaling * top_radius_;

	// After the projection into the xy-plane, the two radii are (x, y) pairs.
	// Compose the vector to the center of each circle from the two radii.
	float right_radius_scale, forward_radius_scale;

	float determinant = 1.0f / (right_radius_trans.x * forward_radius_trans.y - right_radius_trans.y * forward_radius_trans.x);

	right_radius_scale = (-base_center_trans.x*forward_radius_trans.y + base_center_trans.y*forward_radius_trans.x) * determinant;
	forward_radius_scale = (base_center_trans.x*right_radius_trans.y - base_center_trans.y*right_radius_trans.x) * determinant;
	bool base_collision = (pow(right_radius_scale, 2) + pow(forward_radius_scale, 2)) < pow(scaled_base_radius, 2);
	float base_collision_distance = -(right_radius_scale * right_radius_trans.z + forward_radius_scale * forward_radius_trans.z + base_center_trans.z);

	right_radius_scale = (-top_center_trans.x*forward_radius_trans.y + top_center_trans.y*forward_radius_trans.x) * determinant;
	forward_radius_scale = (top_center_trans.x*right_radius_trans.y - top_center_trans.y*right_radius_trans.x) * determinant;
	bool top_collision = (pow(right_radius_scale, 2) + pow(forward_radius_scale, 2)) < pow(scaled_top_radius, 2);
	float top_collision_distance = -(right_radius_scale * right_radius_trans.z + forward_radius_scale * forward_radius_trans.z + top_center_trans.z);

	if (top_collision && base_collision) {
		float smaller_z = std::min(base_collision_distance, top_collision_distance);
		float larger_z = std::max(base_collision_distance, top_collision_distance);
		if (smaller_z < 0 && larger_z >= 0) {
			return larger_z;
		}
		else {
			return smaller_z;
		}
	}

	bool circle_collision = top_collision || base_collision;
	float circle_collision_distance;
	if (top_collision) {
		circle_collision_distance = top_collision_distance;
	}
	else {
		circle_collision_distance = base_collision_distance;
	}

	bool edge_collision;
	float edge_collision_distance;

	if (base_collision != top_collision) {
		edge_collision = true;
	}
	else {
		// Now we need to check if there is an intersection with the edge of the frustum.
		// If there is an intersection with one of the bases but not the other then there must be an intersection with the edge.
		// However, if there is no intersection with either circle there could still be an intersection with the edge.
		// This can be detected by finding the quad in the xy-plane formed by the points on each circle with the max and min y-values.
		// (Or the max and min x-values in the case where the circles are just lines in the xy-plane).
		if (right_radius_trans.y != 0) {
			right_radius_scale = sqrtf(1.0f / (1.0f + pow(forward_radius_trans.y, 2.0f) / pow(right_radius_trans.y, 2.0f)));
			forward_radius_scale = sqrtf(1.0f / (1.0f + pow(right_radius_trans.y, 2.0f) / pow(forward_radius_trans.y, 2.0f)));

			pair<float, float> base_tip_max = std::make_pair(base_center_trans.x + scaled_base_radius * right_radius_scale * right_radius_trans.x, base_center_trans.y + scaled_base_radius * right_radius_scale * right_radius_trans.y);
			pair<float, float> base_tip_min = std::make_pair(base_center_trans.x + scaled_base_radius * forward_radius_scale * forward_radius_trans.x, base_center_trans.y + scaled_base_radius * forward_radius_scale * forward_radius_trans.y);
			pair<float, float> top_tip_max = std::make_pair(top_center_trans.x + scaled_top_radius * right_radius_scale * right_radius_trans.x, top_center_trans.y + scaled_top_radius * right_radius_scale * right_radius_trans.y);
			pair<float, float> top_tip_min = std::make_pair(top_center_trans.x + scaled_top_radius * forward_radius_scale * forward_radius_trans.x, top_center_trans.y + scaled_top_radius * forward_radius_scale * forward_radius_trans.y);
			tuple<bool, bool, bool, bool> intercepts = GetIntercepts(base_tip_max, base_tip_min);
			CombineIntercepts(intercepts, GetIntercepts(base_tip_min, top_tip_min));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_min, top_tip_max));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_max, base_tip_max));

			//std::cout << base_tip_min.first << ", " << base_tip_min.second << " \t " << base_tip_max.first << ", " << base_tip_max.second << std::endl;

			edge_collision = (std::get<0>(intercepts) && std::get<1>(intercepts) && std::get<2>(intercepts) && std::get<3>(intercepts));
		}
		else {
			// The radius is parallel to the x-axis, so we can just use <1, 0> as the radius
			pair<float, float> base_tip_max = std::make_pair(base_center_trans.x + scaled_base_radius, base_center_trans.y);
			pair<float, float> base_tip_min = std::make_pair(base_center_trans.x - scaled_base_radius, base_center_trans.y);
			pair<float, float> top_tip_max = std::make_pair(top_center_trans.x + scaled_top_radius, top_center_trans.y);
			pair<float, float> top_tip_min = std::make_pair(top_center_trans.x - scaled_top_radius, top_center_trans.y);
			tuple<bool, bool, bool, bool> intercepts = GetIntercepts(base_tip_max, base_tip_min);
			CombineIntercepts(intercepts, GetIntercepts(base_tip_min, top_tip_min));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_min, top_tip_max));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_max, base_tip_max));

			//std::cout << base_tip_min.first << ", " << base_tip_min.second << " \t " << base_tip_max.first << ", " << base_tip_max.second << std::endl;

			edge_collision = (std::get<0>(intercepts) && std::get<1>(intercepts) && std::get<2>(intercepts) && std::get<3>(intercepts));

		}
	}

	if (edge_collision) {
		// Need to find the source of the cone as well as the angle it encompases.
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(base_center_trans_vec, top_center_trans_vec);
		float base_to_top_distance;
		DirectX::XMStoreFloat(&base_to_top_distance, DirectX::XMVector3Length(V));
		V = DirectX::XMVector3Normalize(V);
		// The cone direction vector points with unit length from the tip through the top to the base.
		// To find the tip, invert the cone direction vector and scale it so its length is the distance from the base to the tip.
		float base_to_tip_distance = base_to_top_distance + base_to_top_distance / (scaled_base_radius / scaled_top_radius - 1);
		DirectX::XMVECTOR T = DirectX::XMVectorAdd(base_center_trans_vec, DirectX::XMVectorScale(V, -base_to_tip_distance));
		if (DirectX::XMVectorGetZ(V) != 0) {
			float k = scaled_base_radius / base_to_tip_distance;
			float VdotT = DirectX::XMVectorGetX(DirectX::XMVector3Dot(T, V));
			float A = DirectX::XMVectorGetZ(T) - (VdotT / DirectX::XMVectorGetZ(V));
			float B = DirectX::XMVectorGetZ(V) - 1 / DirectX::XMVectorGetZ(V);
			float a = -powf(k, 2) + powf(DirectX::XMVectorGetX(V), 2) + powf(DirectX::XMVectorGetY(V), 2) + powf(B, 2);
			float b = -2 * (DirectX::XMVectorGetX(T) * DirectX::XMVectorGetX(V) + DirectX::XMVectorGetY(T) * DirectX::XMVectorGetY(V) + A * B);
			float c = powf(A, 2) + powf(DirectX::XMVectorGetX(T), 2) + powf(DirectX::XMVectorGetY(T), 2);
			float min_s = (b + sqrtf(powf(b, 2) - 4 * a * c)) / (2 * a);
			float min_z = -(VdotT + min_s) / DirectX::XMVectorGetZ(V);
			float max_s = (b - sqrtf(powf(b, 2) - 4 * a * c)) / (2 * a);
			float max_z = -(VdotT + max_s) / DirectX::XMVectorGetZ(V);

			bool min_valid = (min_s <= base_to_tip_distance && min_s >= (base_to_tip_distance - base_to_top_distance));
			bool max_valid = (max_s <= base_to_tip_distance && max_s >= (base_to_tip_distance - base_to_top_distance));

			if (min_valid && max_valid) {
				float smaller_z = std::min(min_z, max_z);
				float larger_z = std::max(min_z, max_z);
				if (smaller_z < 0 && larger_z >= 0) {
					edge_collision_distance = larger_z;
				}
				else {
					edge_collision_distance = smaller_z;
				}
			}
			else if (min_valid) {
				edge_collision_distance = min_z;;
			}
			else if (max_valid) {
				edge_collision_distance = max_z;
			}
			else {
				return INFINITY;
			}
		}
		else {
			float s = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(T, V));
			float z_base = DirectX::XMVectorGetZ(T) + s * DirectX::XMVectorGetZ(V);
			float z_mod = DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMVectorAdd(DirectX::XMVectorScale(V, s), T)));

			float smaller_z = std::min(-z_base - z_mod, -z_base + z_mod);
			float larger_z = std::max(-z_base - z_mod, -z_base + z_mod);
			if (smaller_z < 0 && larger_z >= 0) {
				edge_collision_distance = larger_z;
			}
			else {
				edge_collision_distance = smaller_z;
			}
		}
	}

	if (!circle_collision) {
		return edge_collision_distance;
	}

	float smaller_z = std::min(edge_collision_distance, circle_collision_distance);
	float larger_z = std::max(edge_collision_distance, circle_collision_distance);
	if (smaller_z < 0 && larger_z >= 0) {
		return larger_z;
	}
	else {
		return smaller_z;
	}
}

tuple<float, std::array<float, 2>> XM_CALLCONV InteractableFrustum::WhereLookedAt(const DirectX::FXMMATRIX& view_transformation) const {
	DirectX::XMMATRIX model_transformation = GetModelTransformation();
	DirectX::XMVECTOR base_center_trans_vec;
	DirectX::XMVECTOR top_center_trans_vec;
	DirectX::XMFLOAT4 base_center_trans;
	DirectX::XMFLOAT4 top_center_trans;
	DirectX::XMFLOAT4 right_radius_trans;
	DirectX::XMFLOAT4 forward_radius_trans;
	base_center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(base_center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&base_center_trans, base_center_trans_vec);
	top_center_trans_vec = DirectX::XMVector4Transform(DirectX::XMVector4Transform(top_center_, model_transformation), view_transformation);
	DirectX::XMStoreFloat4(&top_center_trans, top_center_trans_vec);
	DirectX::XMStoreFloat4(&right_radius_trans, DirectX::XMVectorSubtract(DirectX::XMVector4Transform(DirectX::XMVector4Transform(right_radius_, model_transformation), view_transformation), base_center_trans_vec));
	DirectX::XMStoreFloat4(&forward_radius_trans, DirectX::XMVectorSubtract(DirectX::XMVector4Transform(DirectX::XMVector4Transform(forward_radius_, model_transformation), view_transformation), base_center_trans_vec));

	float matrix_scaling = sqrtf(powf(right_radius_trans.x, 2) + powf(right_radius_trans.y, 2) + powf(right_radius_trans.z, 2));
	float scaled_base_radius = matrix_scaling * base_radius_;
	float scaled_top_radius = matrix_scaling * top_radius_;

	// After the projection into the xy-plane, the two radii are (x, y) pairs.
	// Compose the vector to the center of each circle from the two radii.
	float right_radius_scale, forward_radius_scale;

	float determinant = 1.0f / (right_radius_trans.x * forward_radius_trans.y - right_radius_trans.y * forward_radius_trans.x);

	right_radius_scale = (-base_center_trans.x*forward_radius_trans.y + base_center_trans.y*forward_radius_trans.x) * determinant;
	forward_radius_scale = (base_center_trans.x*right_radius_trans.y - base_center_trans.y*right_radius_trans.x) * determinant;
	bool base_collision = (pow(right_radius_scale, 2) + pow(forward_radius_scale, 2)) < pow(scaled_base_radius, 2);
	float base_collision_distance = -(right_radius_scale * right_radius_trans.z + forward_radius_scale * forward_radius_trans.z + base_center_trans.z);

	right_radius_scale = (-top_center_trans.x*forward_radius_trans.y + top_center_trans.y*forward_radius_trans.x) * determinant;
	forward_radius_scale = (top_center_trans.x*right_radius_trans.y - top_center_trans.y*right_radius_trans.x) * determinant;
	bool top_collision = (pow(right_radius_scale, 2) + pow(forward_radius_scale, 2)) < pow(scaled_top_radius, 2);
	float top_collision_distance = -(right_radius_scale * right_radius_trans.z + forward_radius_scale * forward_radius_trans.z + top_center_trans.z);

	if (top_collision && base_collision) {
		float smaller_z = std::min(base_collision_distance, top_collision_distance);
		float larger_z = std::max(base_collision_distance, top_collision_distance);
		if (smaller_z < 0 && larger_z >= 0) {
			return std::make_tuple(larger_z, array<float, 2>({ { 0, 0 } }));
		}
		else {
			return std::make_tuple(smaller_z, array<float, 2>({ { 0, 0 } }));
		}
	}

	bool circle_collision = top_collision || base_collision;
	float circle_collision_distance;
	if (top_collision) {
		circle_collision_distance = top_collision_distance;
	} else {
		circle_collision_distance = base_collision_distance;
	}

	bool edge_collision;
	float edge_collision_distance;

	if (base_collision != top_collision) {
		edge_collision = true;
	}
	else {
		// Now we need to check if there is an intersection with the edge of the frustum.
		// If there is an intersection with one of the bases but not the other then there must be an intersection with the edge.
		// However, if there is no intersection with either circle there could still be an intersection with the edge.
		// This can be detected by finding the quad in the xy-plane formed by the points on each circle with the max and min y-values.
		// (Or the max and min x-values in the case where the circles are just lines in the xy-plane).
		if (right_radius_trans.y != 0) {
			right_radius_scale = sqrtf(1.0f / (1.0f + pow(forward_radius_trans.y, 2.0f) / pow(right_radius_trans.y, 2.0f)));
			forward_radius_scale = sqrtf(1.0f / (1.0f + pow(right_radius_trans.y, 2.0f) / pow(forward_radius_trans.y, 2.0f)));

			pair<float, float> base_tip_max = std::make_pair(base_center_trans.x + scaled_base_radius * right_radius_scale * right_radius_trans.x, base_center_trans.y + scaled_base_radius * right_radius_scale * right_radius_trans.y);
			pair<float, float> base_tip_min = std::make_pair(base_center_trans.x + scaled_base_radius * forward_radius_scale * forward_radius_trans.x, base_center_trans.y + scaled_base_radius * forward_radius_scale * forward_radius_trans.y);
			pair<float, float> top_tip_max = std::make_pair(top_center_trans.x + scaled_top_radius * right_radius_scale * right_radius_trans.x, top_center_trans.y + scaled_top_radius * right_radius_scale * right_radius_trans.y);
			pair<float, float> top_tip_min = std::make_pair(top_center_trans.x + scaled_top_radius * forward_radius_scale * forward_radius_trans.x, top_center_trans.y + scaled_top_radius * forward_radius_scale * forward_radius_trans.y);
			tuple<bool, bool, bool, bool> intercepts = GetIntercepts(base_tip_max, base_tip_min);
			CombineIntercepts(intercepts, GetIntercepts(base_tip_min, top_tip_min));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_min, top_tip_max));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_max, base_tip_max));

			//std::cout << base_tip_min.first << ", " << base_tip_min.second << " \t " << base_tip_max.first << ", " << base_tip_max.second << std::endl;

			edge_collision = (std::get<0>(intercepts) && std::get<1>(intercepts) && std::get<2>(intercepts) && std::get<3>(intercepts));
		} else {
			// The radius is parallel to the x-axis, so we can just use <1, 0> as the radius
			pair<float, float> base_tip_max = std::make_pair(base_center_trans.x + scaled_base_radius, base_center_trans.y);
			pair<float, float> base_tip_min = std::make_pair(base_center_trans.x - scaled_base_radius, base_center_trans.y);
			pair<float, float> top_tip_max = std::make_pair(top_center_trans.x + scaled_top_radius, top_center_trans.y);
			pair<float, float> top_tip_min = std::make_pair(top_center_trans.x - scaled_top_radius, top_center_trans.y);
			tuple<bool, bool, bool, bool> intercepts = GetIntercepts(base_tip_max, base_tip_min);
			CombineIntercepts(intercepts, GetIntercepts(base_tip_min, top_tip_min));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_min, top_tip_max));
			CombineIntercepts(intercepts, GetIntercepts(top_tip_max, base_tip_max));

			//std::cout << base_tip_min.first << ", " << base_tip_min.second << " \t " << base_tip_max.first << ", " << base_tip_max.second << std::endl;

			edge_collision = (std::get<0>(intercepts) && std::get<1>(intercepts) && std::get<2>(intercepts) && std::get<3>(intercepts));

		}
	}

	if (edge_collision) {
		// Need to find the source of the cone as well as the angle it encompases.
		DirectX::XMVECTOR V = DirectX::XMVectorSubtract(base_center_trans_vec, top_center_trans_vec);
		float base_to_top_distance;
		DirectX::XMStoreFloat(&base_to_top_distance, DirectX::XMVector3Length(V));
		V = DirectX::XMVector3Normalize(V);
		// The cone direction vector points with unit length from the tip through the top to the base.
		// To find the tip, invert the cone direction vector and scale it so its length is the distance from the base to the tip.
		float base_to_tip_distance = base_to_top_distance + base_to_top_distance / (scaled_base_radius / scaled_top_radius - 1);
		DirectX::XMVECTOR T = DirectX::XMVectorAdd(base_center_trans_vec, DirectX::XMVectorScale(V, -base_to_tip_distance));
		if (DirectX::XMVectorGetZ(V) != 0) {
			float k = scaled_base_radius / base_to_tip_distance;
			float VdotT = DirectX::XMVectorGetX(DirectX::XMVector3Dot(T, V));
			float A = DirectX::XMVectorGetZ(T) - (VdotT / DirectX::XMVectorGetZ(V));
			float B = DirectX::XMVectorGetZ(V) - 1 / DirectX::XMVectorGetZ(V);
			float a = -powf(k, 2) + powf(DirectX::XMVectorGetX(V), 2) + powf(DirectX::XMVectorGetY(V), 2) + powf(B, 2);
			float b = -2 * (DirectX::XMVectorGetX(T) * DirectX::XMVectorGetX(V) + DirectX::XMVectorGetY(T) * DirectX::XMVectorGetY(V) + A * B);
			float c = powf(A, 2) + powf(DirectX::XMVectorGetX(T), 2) + powf(DirectX::XMVectorGetY(T), 2);
			float min_s = (b + sqrtf(powf(b, 2) - 4 * a * c)) / (2 * a);
			float min_z = -(VdotT + min_s) / DirectX::XMVectorGetZ(V);
			float max_s = (b - sqrtf(powf(b, 2) - 4 * a * c)) / (2 * a);
			float max_z = -(VdotT + max_s) / DirectX::XMVectorGetZ(V);

			bool min_valid = (min_s <= base_to_tip_distance && min_s >= (base_to_tip_distance - base_to_top_distance));
			bool max_valid = (max_s <= base_to_tip_distance && max_s >= (base_to_tip_distance - base_to_top_distance));

			if (min_valid && max_valid) {
				float smaller_z = std::min(min_z, max_z);
				float larger_z = std::max(min_z, max_z);
				if (smaller_z < 0 && larger_z >= 0) {
					edge_collision_distance = larger_z;
				}
				else {
					edge_collision_distance = smaller_z;
				}
			} else if (min_valid) {
				edge_collision_distance = min_z;;
			} else if (max_valid) {
				edge_collision_distance = max_z;
			} else {
				return std::make_tuple(INFINITY, array<float, 2>({ { 0, 0 } }));
			}
		} else {
			float s = -DirectX::XMVectorGetX(DirectX::XMVector3Dot(T, V));
			float z_base = DirectX::XMVectorGetZ(T) + s * DirectX::XMVectorGetZ(V);
			float z_mod = DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(DirectX::XMVectorAdd(DirectX::XMVectorScale(V, s), T)));
			
			float smaller_z = std::min(-z_base - z_mod, -z_base + z_mod);
			float larger_z = std::max(-z_base - z_mod, -z_base + z_mod);
			if (smaller_z < 0 && larger_z >= 0) {
				edge_collision_distance = larger_z;
			}
			else {
				edge_collision_distance = smaller_z;
			}
		}
	}

	if (!circle_collision) {
		return std::make_tuple(edge_collision_distance, array<float, 2>({ { 0, 0 } }));
	}

	float smaller_z = std::min(edge_collision_distance, circle_collision_distance);
	float larger_z = std::max(edge_collision_distance, circle_collision_distance);
	if (smaller_z < 0 && larger_z >= 0) {
		return std::make_tuple(larger_z, array<float, 2>({ { 0, 0 } }));
	} else {
		return std::make_tuple(smaller_z, array<float, 2>({ { 0, 0 } }));
	}
}

tuple<bool, bool, bool, bool> InteractableFrustum::GetIntercepts(pair<float, float> p1, pair<float, float> p2) {
	float y_intercept, x_intercept;
	if (p2.second == p1.second) {
		x_intercept = std::nanf("");
	} else {
		x_intercept = p1.first - (p1.second / (p2.second - p1.second) * (p2.first - p1.first));
	}
	if (p2.first == p1.first) {
		y_intercept = std::nanf("");
	} else {
		y_intercept = p1.second - (p1.first / (p2.first - p1.first) * (p2.second - p1.second));
	}
	return std::make_tuple(y_intercept <= 0, y_intercept >= 0, x_intercept <= 0, x_intercept >= 0);
}

void InteractableFrustum::CombineIntercepts(tuple<bool, bool, bool, bool>& existing_intercepts, tuple<bool, bool, bool, bool> new_intercepts) {
	std::get<0>(existing_intercepts) |= std::get<0>(new_intercepts);
	std::get<1>(existing_intercepts) |= std::get<1>(new_intercepts);
	std::get<2>(existing_intercepts) |= std::get<2>(new_intercepts);
	std::get<3>(existing_intercepts) |= std::get<3>(new_intercepts);
}