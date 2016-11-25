#pragma once
#include "stdafx.h"

#include "PythonClassHelpers.h"
#include "StlHelper.h"

void StlInterface() {
	CreateArraysWithVector<int, 3>::Create("Int");
	CreateVector<unsigned char>("UnsignedChar");
	CreateVector<float>("Float");

	CreateArraysWithVector<float, 12>::Create("Float");

	CreateMap<string, float>("String", "Float");

	CreateArray<bool, 2>("Bool");
}