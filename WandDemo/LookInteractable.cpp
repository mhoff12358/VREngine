#include "stdafx.h"
#include "LookInteractable.h"


LookInteractable::LookInteractable(Identifier id) : id_(id)
{
	model_transformation_ = DirectX::XMMatrixIdentity();
}


LookInteractable::~LookInteractable()
{
}

void XM_CALLCONV LookInteractable::SetModelTransformation(DirectX::FXMMATRIX model_transformation) {
	model_transformation_ = model_transformation;
}

const Identifier& LookInteractable::GetId() {
	return id_;
}