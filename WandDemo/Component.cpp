#include "stdafx.h"
#include "Component.h"

Component::Component(EntityHandler& entity_handler, ID3D11Device* device_interface, vector<Model> models) : Component(entity_handler, device_interface, models.begin(), models.end()) {

}

Component::Component(EntityHandler& entity_handler, ID3D11Device* device_interface, vector<Model>::iterator first_model, vector<Model>::iterator end_of_models)
	: entity_handler_(entity_handler), transformation_buffer_(CB_PS_VERTEX_SHADER), children_(NULL), parent_transformation_(NULL), num_children_(0)
{
	transformation_buffer_.CreateBuffer(device_interface);

	number_of_entities_ = std::distance(first_model, end_of_models);
	assert(number_of_entities_ > 0);

	//ConstantBufferTyped<TransformationMatrixAndInvTransData>* trans = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	//trans->CreateBuffer(device_interface);
	vector<Model>::iterator current_model = first_model;
	first_entity_ = entity_handler_.AddEntity(Entity(
		ES_NORMAL,
		PixelShader(),
		VertexShader(),
		ShaderSettings(NULL),
		*current_model,
		&transformation_buffer_));
		//trans));
	for (++current_model; current_model != end_of_models; ++current_model) {
		entity_handler_.AddEntity(Entity(
			ES_NORMAL,
			PixelShader(),
			VertexShader(),
			ShaderSettings(NULL),
			*current_model,
			NULL));
	}
}

Component::~Component()
{
}

Component::Component(Component&& other)
	: entity_handler_(other.entity_handler_), first_entity_(other.first_entity_),
	number_of_entities_(other.number_of_entities_), children_(other.children_), num_children_(other.num_children_),
	transformation_buffer_(std::move(other.transformation_buffer_)), local_transformation_(other.local_transformation_),
	combined_transformation_(other.combined_transformation_), parent_transformation_(other.parent_transformation_)
{

}

void Component::SetChildren(Component* children, int num_children) {
	children_ = children;
	num_children_ = num_children;
	for (int i = 0; i < num_children_; i++) {
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