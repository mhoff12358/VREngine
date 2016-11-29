#include "stdafx.h"
#include "StlInterface.h"

#include "PythonClassHelpers.h"
#include "StlHelper.h"

void StlInterface() {
	CreateArraysWithVector<int, 3>::Create("Int");
	CreateArraysWithVector<bool, 3>::Create("Bool");
	CreateArraysWithVector<float, 12>::Create("Float");

	CreateVector<unsigned char>("UnsignedChar");
	CreateVector<float>("Float");

	CreateMap<string, float>("String", "Float");
}