#pragma once

#include <vector>
using std::vector;

#include "EntityHandler.h"
#include "Entity.h"
#include "Model.h"
#include "ConstantBuffer.h"

class Component
{
public:
	Component(EntityHandler& entity_handler, ID3D11Device* device_interface, const vector<Model>& models);
	~Component();

	void SetChildren(Component* children, unsigned int num_children);
	
	void XM_CALLCONV SetLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void UpdateTransformation();

private:
	void SetParent(Component* parent);

	EntityHandler& entity_handler_;
	
	unsigned int first_entity_;
	unsigned int number_of_entities_;

	Component* children_;
	unsigned int num_children_;

	ConstantBufferTyped<TransformationMatrixAndInvTransData> transformation_buffer_;

	DirectX::XMMATRIX local_transformation_;
	DirectX::XMMATRIX combined_transformation_;
	DirectX::XMMATRIX* parent_transformation_;
};

