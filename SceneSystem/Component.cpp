#include "stdafx.h"
#include "Component.h"

Component::Component(ID3D11Device* device_interface)
	: transformation_buffer_(ShaderStages(ShaderStages::VERTEX_STAGE | ShaderStages::GEOMETRY_STAGE)), children_(NULL), parent_transformation_(NULL), num_children_(0)
{
	transformation_buffer_.CreateBuffer(device_interface);
}

Component::Component()
	: transformation_buffer_(ShaderStages()) {
}

void Component::AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model> models) {
	AddEntitiesToHandler(entity_handler, entity_group_name, models.begin(), models.end());
}

void Component::AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model>::iterator first_model, vector<Model>::iterator end_of_models) {
	number_of_entities_ = std::distance(first_model, end_of_models);
	assert(number_of_entities_ > 0);

	//ConstantBufferTyped<TransformationMatrixAndInvTransData>* trans = new ConstantBufferTyped<TransformationMatrixAndInvTransData>(CB_PS_VERTEX_SHADER);
	//trans->CreateBuffer(device_interface);
	vector<Model>::iterator current_model = first_model;
	first_entity_ = entity_handler.AddEntity(Entity(
		ES_NORMAL,
		Shaders(),
		ShaderSettings(NULL),
		*current_model,
		&transformation_buffer_), entity_group_name);
	//trans));
	for (++current_model; current_model != end_of_models; ++current_model) {
		entity_handler.AddEntity(Entity(
			ES_NORMAL,
			Shaders(),
			ShaderSettings(NULL),
			*current_model,
			NULL), entity_group_name);
	}
}

Component::~Component()
{
}

Component::Component(Component&& other)
	: first_entity_(other.first_entity_),
	number_of_entities_(other.number_of_entities_), children_(other.children_), num_children_(other.num_children_),
	transformation_buffer_(std::move(other.transformation_buffer_)), local_transformation_(other.local_transformation_),
	combined_transformation_(other.combined_transformation_), parent_transformation_(other.parent_transformation_)
{

}

void Component::SetChildren(Component* children, int num_children) {
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

void XM_CALLCONV Component::RightComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update) {
	local_transformation_ = DirectX::XMMatrixMultiply(local_transformation_, new_transformation);
	if (apply_update) {
		UpdateTransformation();
	}
}

void XM_CALLCONV Component::LeftComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update) {
	local_transformation_ = DirectX::XMMatrixMultiply(new_transformation, local_transformation_);
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

const TransformationMatrixAndInvTransData* Component::GetTransformationData() {
	return transformation_buffer_.ReadBufferDataTyped();
}

ConstantBuffer* Component::GetTransformationBuffer() {
	return dynamic_cast<ConstantBuffer*>(&transformation_buffer_);
}

const DirectX::XMMATRIX& Component::GetLocalTransformation() {
	return local_transformation_;
}

const DirectX::XMMATRIX& Component::GetCombinedTransformation() {
	return combined_transformation_;
}
