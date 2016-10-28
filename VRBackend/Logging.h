#ifndef __LOGGING_H_INCLUDED__
#define __LOGGING_H_INCLUDED__

#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

void OutputFormatted(const char* format, ...);

std::string floatToString(float a);

std::string doubleToString(double a);

std::string floatTripleToString(float a, float b, float c);

std::string intToString(int a);

void dumpMatrix(DirectX::XMFLOAT4X4 matrix);

#endif