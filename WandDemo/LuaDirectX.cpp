#include "stdafx.h"
#include "LuaDirectX.h"

DirectX::XMMATRIX LuaTableToMatrix(Lua::Environment environment_with_table) {
	string matrix_type;
	if (environment_with_table.LoadFromTable(string("matrix_type"), &matrix_type)) {
		if (matrix_type == "translation") {
			float translation[3];
			if (environment_with_table.LoadFromTable(string("x"), translation) &&
				environment_with_table.LoadFromTable(string("y"), translation + 1) &&
				environment_with_table.LoadFromTable(string("z"), translation + 2)) {
				return DirectX::XMMatrixTranslation(translation[0], translation[1], translation[2]);
			}
		}
		else if (matrix_type == "axis_rotation") {
			float axis[3];
			float rotation;
			if (environment_with_table.LoadFromTable(string("x"), axis) &&
				environment_with_table.LoadFromTable(string("y"), axis + 1) &&
				environment_with_table.LoadFromTable(string("z"), axis + 2) &&
				environment_with_table.LoadFromTable(string("rotation"), &rotation)) {
				return DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(axis[0], axis[1], axis[2], 0), rotation);
			}
		}
	}
	return DirectX::XMMatrixIdentity();
}
