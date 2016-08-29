#include "Logging.h"

void OutputFormatted(const char* format, ...) {
	static char output_string[2048];
	va_list args;
	va_start(args, format);
	if (format != NULL && *format != 0) {
		vsprintf_s(output_string, format, args);
		OutputDebugString(output_string);
	}
	OutputDebugString("\n");
}

std::string floatToString(float a) {
	std::ostringstream ss;
	ss << a;
	return ss.str();
}

std::string doubleToString(double a) {
	std::ostringstream ss;
	ss << a;
	return ss.str();
}

std::string floatTripleToString(float a, float b, float c) {
	return floatToString(a) + ", " + floatToString(b) + ", " + floatToString(c);
}

std::string intToString(int a) {
	std::ostringstream ss;
	ss << a;
	return ss.str();
}

void dumpMatrix(DirectX::XMFLOAT4X4 matrix) {
	auto data = matrix.m;
	OutputFormatted("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f",
		data[0][0], data[0][1], data[0][2], data[0][3],
		data[1][0], data[1][1], data[1][2], data[1][3],
		data[2][0], data[2][1], data[2][2], data[2][3],
		data[3][0], data[3][1], data[3][2], data[3][3]);
}

HRESULT HandleHResult(HRESULT result) {
	if (!SUCCEEDED(result)) {
		std::ostringstream ss;
		ss << "Failing HR code: ";
		ss << std::hex << std::setfill('0');
		ss << std::setw(8) << result;
		ss << std::endl;
		OutputDebugString(ss.str().c_str());
	}
	return result;
}