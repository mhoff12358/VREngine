#ifndef __LOGGING_H_INCLUDED__
#define __LOGGING_H_INCLUDED__

#include "Windows.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#include "DirectXMath.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include "D3D11.h"

void OutputFormatted(const char* format, ...);

std::string floatToString(float a);

std::string doubleToString(double a);

std::string floatTripleToString(float a, float b, float c);

std::string intToString(int a);

void dumpMatrix(DirectX::XMFLOAT4X4 matrix);

HRESULT HandleHResult(HRESULT result);

#endif