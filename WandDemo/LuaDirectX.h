#pragma once

#include "stdafx.h"

#include <DirectXMath.h>

#include "LuaGameScripting/Environment.h"

// The lua environment should have a table at the provided index, which will be turned into the matrix.
DirectX::XMMATRIX LuaTableToMatrix(Lua::Environment environment_with_table);
