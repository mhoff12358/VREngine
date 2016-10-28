#pragma once

#include "stdafx.h"

#include "ModelSlice.h"

class ModelMutation;

class Model
{
public:
	Model();
	Model(ID3D11Buffer* vb, UINT st, UINT off, D3D_PRIMITIVE_TOPOLOGY prim_type, ModelSlice ms, bool own_vert_buff = false);
	~Model();

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const;
	bool IsDummy() const;

	void ApplyModelMutation(ID3D11DeviceContext* device_context, const ModelMutation& mutation);

	ModelSlice slice;

private:
	ID3D11Buffer* vertex_buffer;
	bool owns_vertex_buffer;
	UINT stride;
	UINT offset;
	D3D_PRIMITIVE_TOPOLOGY primitive_type;
};

