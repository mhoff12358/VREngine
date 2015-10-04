#include "stdafx.h"
#include "Component.h"


Component::Component(EntityHandler& entity_handler, ID3D11Device* device_interface, const vector<Model>& models)
	: entity_handler_(entity_handler), transformation_buffer_(CB_PS_VERTEX_SHADER), children_(NULL), num_children_(0)
{
	transformation_buffer_.CreateBuffer(device_interface);

	first_entity_ = entity_handler_.AddEntity(Entity(
		ES_SETTINGS,
		PixelShader(),
		VertexShader(),
		ShaderSettings(NULL),
		Model(),
		&transformation_buffer_));
	for (const Model& model : models) {
		entity_handler_.AddEntity(Entity(
			ES_NORMAL,
			PixelShader(),
			VertexShader(),
			ShaderSettings(NULL),
			model,
			NULL));
	}
	number_of_entities_ = models.size() + 1;
}

Component::~Component()
{
}

void Component::SetChildren(Component* children, unsigned int num_children) {
	children_ = children;
	num_children_ = num_children;
	for (unsigned int i = 0; i < num_children_; i++) {
		children_[i].SetParent(this);
	}
}

void Component::SetParent(Component* parent) {
	parent_transformation_ = &parent->combined_transformation_;
}

void XM_CALLCONV Component::SetLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update) {
	local_transformation_ = new_transformation;
	if (apply_update) {
		UpdateTransformation();
	}
}

void Component::UpdateTransformation() {
	if (parent_transformation_ != NULL) {
		combined_transformation_ = DirectX::XMMatrixMultiply(local_transformation_, *parent_transformation_);
	} else {
		combined_transformation_ = local_transformation_;
	}
	transformation_buffer_.SetBothTransformations(combined_transformation_);

	for (unsigned int i = 0; i < num_children_; i++) {
		children_[i].UpdateTransformation();
	}
}