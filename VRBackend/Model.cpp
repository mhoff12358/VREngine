#include "stdafx.h"
#include "Model.h"

#include "ModelMutation.h"

Model::Model()
	: vertex_buffer(NULL), slice(0, 0)
{
}

Model::Model(ID3D11Buffer* vb, UINT st, UINT off, D3D_PRIMITIVE_TOPOLOGY prim_type, ModelSlice ms, bool own_vert_buff)
	: vertex_buffer(vb), stride(st), offset(off), primitive_type(prim_type), slice(ms), owns_vertex_buffer(own_vert_buff) {

}

Model::~Model()
{
}

void Model::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (!IsDummy()) {
		device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
		device_context->IASetPrimitiveTopology(primitive_type);
	}
}

bool Model::IsDummy() const {
	return vertex_buffer == NULL;
}

void Model::ApplyModelMutation(ID3D11DeviceContext* device_context, const ModelMutation& model_mutation) {
	D3D11_MAPPED_SUBRESOURCE mapped_result;
	if (device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE, 0, &mapped_result) == S_OK) {
		model_mutation.ApplyToData((char*)mapped_result.pData);
	} else {
		std::cout << "FAILED ATTEMPT AT MAPPING VERTEX BUFFER" << std::endl;
	}
	device_context->Unmap(vertex_buffer, 0);
}