#ifndef __PERSPECTIVE_CAMERA_H_INCLUDED__
#define __PERSPECTIVE_CAMERA_H_INCLUDED__

#include <array>

#include "DirectXMath.h"

#include "Camera.h"
#include "ViewState.h"

class PerspectiveCamera : public Camera {
public:
	PerspectiveCamera();

	void BuildProjectionMatrix() override;

	float vertical_fov;
	float aspect_ratio;
};

#endif