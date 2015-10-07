#include "stdafx.h"
#include "LookInteractable.h"


LookInteractable::LookInteractable()
{
	model_transformation_ = DirectX::XMMatrixIdentity();
}


LookInteractable::~LookInteractable()
{
}

void XM_CALLCONV LookInteractable::SetModelTransformation(const DirectX::FXMMATRIX& model_transformation) {
	model_transformation_ = model_transformation;
}