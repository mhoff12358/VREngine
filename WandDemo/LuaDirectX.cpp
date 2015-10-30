#include "stdafx.h"
#include "LuaDirectX.h"

DirectX::XMMATRIX LuaTableToMatrix(Lua::Environment environment_with_table) {
	int table_length_as_array = environment_with_table.GetArrayLength();
	if (table_length_as_array == 0) {
		string matrix_type;
		if (environment_with_table.LoadFromTable(string("matrix_type"), &matrix_type)) {
			if (matrix_type == "translation") {
				float translation[3];
				if (environment_with_table.LoadFromTable(string("x"), translation) &&
					environment_with_table.LoadFromTable(string("y"), translation + 1) &&
					environment_with_table.LoadFromTable(string("z"), translation + 2)) {
					return DirectX::XMMatrixTranslation(translation[0], translation[1], translation[2]);
				}
			} else if (matrix_type == "axis_rotation") {
				float axis[3];
				float rotation;
				if (environment_with_table.LoadFromTable(string("x"), axis) &&
					environment_with_table.LoadFromTable(string("y"), axis + 1) &&
					environment_with_table.LoadFromTable(string("z"), axis + 2) &&
					environment_with_table.LoadFromTable(string("rotation"), &rotation)) {
					return DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(axis[0], axis[1], axis[2], 0), rotation);
				}
			} else if (matrix_type == "quaternion_rotation") {
				array<float, 4> quaternion_array;
				if (environment_with_table.LoadFromTable(string("quaternion"), &quaternion_array)) {
					DirectX::XMFLOAT4 quaternion(quaternion_array.data());
					return DirectX::XMMatrixRotationQuaternion(
						DirectX::XMLoadFloat4(&quaternion));
				}
			} else if (matrix_type == "scale") {
				float scales[3];
				if (environment_with_table.LoadArrayFromTable(string("scale"), scales, environment_with_table.stack_top, 3)) {
					return DirectX::XMMatrixScaling(scales[0], scales[1], scales[2]);
				}
			}
		}
		return DirectX::XMMatrixIdentity();
	} else {
		DirectX::XMMATRIX built_matrix = DirectX::XMMatrixIdentity();
		for (int i = 1; i <= table_length_as_array; i++) {
			environment_with_table.GetFromTableToStack(i);
			built_matrix = DirectX::XMMatrixMultiply(built_matrix, LuaTableToMatrix(environment_with_table));
			environment_with_table.RemoveFromStack();
		}
		return built_matrix;
	}
}
