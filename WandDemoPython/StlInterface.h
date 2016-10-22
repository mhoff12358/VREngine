#pragma once
#include "stdafx.h"

#include "PythonClassHelpers.h"
#include "StlHelper.h"

void StlInterface() {
	CreateArray<int, 2>("Int");
	CreateVector<unsigned char>("UnsignedChar");
	CreateVector<float>("Float");

	CreateArraysWithVector<float, 12>("Float");
}