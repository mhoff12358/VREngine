#include "stdafx.h"
#include "LookInteractable.h"


LookInteractable::LookInteractable(Identifier id, Actor* actor) : id_(id), actor_(actor), model_transformation_(NULL)
{
}


LookInteractable::~LookInteractable()
{
}

void LookInteractable::SetModelTransformation(const DirectX::XMFLOAT4X4* model_transformation) {
	model_transformation_ = model_transformation;
}

DirectX::XMMATRIX LookInteractable::GetModelTransformation() const {
	if (model_transformation_ != NULL) {
		return DirectX::XMLoadFloat4x4(model_transformation_);
	}
	else {
		return DirectX::XMMatrixIdentity();
	}
}

const Identifier& LookInteractable::GetId() const {
	return id_;
}

Actor* LookInteractable::GetActor() const {
	return actor_;
}