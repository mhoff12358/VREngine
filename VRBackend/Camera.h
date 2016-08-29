#ifndef __CAMERA_H_INCLUDED__
#define __CAMERA_H_INCLUDED__

#include "DirectXMath.h"

#include "ViewState.h"

class Camera
{
public:
	Camera();
	virtual ~Camera() {};

	virtual void InvalidateAllMatrices();
	virtual void InvalidateViewMatrices();

	virtual void BuildLocationMatrix();
	virtual void BuildOrientationMatrix();
	virtual void BuildProjectionMatrix();

	virtual void BuildViewMatrix();
	virtual void BuildViewProjectionMatrix();
	virtual void BuildOrientationProjectionMatrix();

	virtual DirectX::XMMATRIX GetViewMatrix();
	virtual DirectX::XMMATRIX GetViewProjectionMatrix();
	virtual DirectX::XMMATRIX GetOrientationProjectionMatrix();

	std::array<float, 3> location;
	std::array<float, 4> orientaiton;

protected:
	DirectX::XMFLOAT4X4 projection_matrix;
	DirectX::XMFLOAT4X4 location_matrix;
	DirectX::XMFLOAT4X4 orientation_matrix;

	DirectX::XMFLOAT4X4 view_matrix;
	DirectX::XMFLOAT4X4 view_projection_matrix;
	DirectX::XMFLOAT4X4 orientation_projection_matrix;
};

#endif