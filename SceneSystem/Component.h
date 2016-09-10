#pragma once

#include "stdafx.h"

#include "VRBackend/EntityHandler.h"
#include "VRBackend/Entity.h"
#include "VRBackend/Model.h"
#include "VRBackend/ConstantBuffer.h"

class DLLSTUFF Component
{
public:
	Component(ID3D11Device* device_interface);
	~Component();

	Component(const Component& other) = delete;
	Component operator=(const Component& other) = delete;
	Component(Component&& other);

	void SetChildren(Component* children, int num_children);

	void AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model> models);
	void AddEntitiesToHandler(EntityHandler& entity_handler, string entity_group_name, vector<Model>::iterator first_model, vector<Model>::iterator end_of_models);

	void XM_CALLCONV SetLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV LeftComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void XM_CALLCONV RightComposeLocalTransformation(DirectX::FXMMATRIX new_transformation, bool apply_update = true);
	void UpdateTransformation();
	const TransformationMatrixAndInvTransData* GetTransformationData();
	const DirectX::XMMATRIX& GetLocalTransformation();
	const DirectX::XMMATRIX& GetCombinedTransformation();

	unsigned int first_entity_;
	unsigned int number_of_entities_;

private:
	void SetParent(Component* parent);

	Component* children_;
	unsigned int num_children_;

	ConstantBufferTyped<TransformationMatrixAndInvTransData> transformation_buffer_;

	DirectX::XMMATRIX local_transformation_;
	DirectX::XMMATRIX combined_transformation_;
	DirectX::XMMATRIX* parent_transformation_;
};

