#pragma once

#include "stdafx.h"

#include "EntityHandler.h"
#include "Entity.h"
#include "Model.h"
#include "ConstantBuffer.h"

class Component
{
public:
	Component(EntityHandler& entity_handler, ID3D11Device* device_interface, vector<Model>::iterator first_model, vector<Model>::iterator end_of_models);
	Component(EntityHandler& entity_handler, ID3D11Device* device_interface, vector<Model> models);
	~Component();

	Component(const Component& other) = delete;
	Component operator=(const Component& other) = delete;
	Component(Component&& other);

	void SetChildren(Component* children, int num_children);

	void XM_CALLCONV SetLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV LeftComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV RightComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void UpdateTransformation();
	const TransformationMatrixAndInvTransData* GetTransformationData();

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

