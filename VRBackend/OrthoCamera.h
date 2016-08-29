#ifndef __ORTHO_CAMERA_H_INCLUDED__
#define __ORTHO_CAMERA_H_INCLUDED__

#include "Camera.h"
class OrthoCamera :	public Camera {
public:
	OrthoCamera();

	void BuildProjectionMatrix() override;

	std::array<float, 2> screen_size;
};

#endif