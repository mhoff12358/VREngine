#include "stdafx.h"
#include "StlInterface.h"

#include "PythonClassHelpers.h"
#include "StlHelper.h"

void StlInterface() {
	CreateArray<int, 2>("Int");
	CreateVector<unsigned char>("UnsignedChar");
	CreateVector<float>("Float");

	CreateArraysWithVector<float, 12>::Create("Float");

	CreateMap<string, float>("String", "Float");
}