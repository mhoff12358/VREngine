#include "stdafx.h"
#include "ConstantBuffer.h"

ConstantBuffer::ConstantBuffer(ShaderStages stages) : pipeline_stages(stages)
{
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::CreateBuffer(ID3D11Device* device_interface) {
	D3D11_BUFFER_DESC const_buff_desc;
	const_buff_desc.ByteWidth = GetBufferDataSize();
	const_buff_desc.Usage = D3D11_USAGE_DYNAMIC;
	const_buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	const_buff_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	const_buff_desc.MiscFlags = 0;
	const_buff_desc.StructureByteStride = 0;

	// Create the subresource data that carries the buffer data
	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem = ReadBufferData();
	sub_data.SysMemPitch = 0;
	sub_data.SysMemSlicePitch = 0;

	device_interface->CreateBuffer(&const_buff_desc, &sub_data, &const_buffer);
}

void ConstantBuffer::PushBuffer(ID3D11DeviceContext* device_context) {
	if (dirty) {
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		// Disables GPU access to the data
		device_context->Map(const_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		memcpy(mappedResource.pData, ReadBufferData(), GetBufferDataSize());
		device_context->Unmap(const_buffer, 0);
	}

	dirty = false;
}

void ConstantBuffer::SetDirty() {
	dirty = true;
}

void ConstantBuffer::ForcePushBuffer(ID3D11DeviceContext* device_context) {
	SetDirty();
	PushBuffer(device_context);
}

void ConstantBuffer::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context, int buffer_register) const {
	if (pipeline_stages.IsVertexStage()) {
		device_context->VSSetConstantBuffers(buffer_register, 1, &const_buffer);
	}
	if (pipeline_stages.IsPixelStage()) {
		device_context->PSSetConstantBuffers(buffer_register, 1, &const_buffer);
	}
	if (pipeline_stages.IsGeometryStage()) {
		device_context->GSSetConstantBuffers(buffer_register, 1, &const_buffer);
	}
}

SizedConstantBuffer::SizedConstantBuffer(ShaderStages stages, unsigned int size) : ConstantBuffer(stages) {
	buffer_data.resize(size);
}

SizedConstantBuffer::~SizedConstantBuffer() {

}

void* SizedConstantBuffer::EditBufferData() {
	return EditBufferData(true);
}

void* SizedConstantBuffer::EditBufferData(bool set_dirty) {
	if (set_dirty) {
		SetDirty();
	}
	return buffer_data.data();
}

const void* SizedConstantBuffer::ReadBufferData() {
	return buffer_data.data();
} 

unsigned int SizedConstantBuffer::GetBufferDataSize() {
	return buffer_data.size();
}

void XM_CALLCONV ConstantBufferTyped<TransformationMatrixData>::SetTransformation(DirectX::FXMMATRIX new_trasnformation) {
	DirectX::XMStoreFloat4x4(&buffer_data.transformation, new_trasnformation);
	SetDirty();
}

void XM_CALLCONV ConstantBufferTyped<TransformationMatrixAndInvTransData>::SetBothTransformations(DirectX::FXMMATRIX new_trasnformation) {
	DirectX::XMStoreFloat4x4(&buffer_data.transformation, new_trasnformation);
	DirectX::XMStoreFloat4x4(&buffer_data.transformation_inv_trans,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(NULL, new_trasnformation)));
	SetDirty();
}

void XM_CALLCONV ConstantBufferTyped<TransformationMatrixAndInvTransData>::SetTransformation(DirectX::FXMMATRIX new_trasnformation) {
	DirectX::XMStoreFloat4x4(&buffer_data.transformation, new_trasnformation);
	SetDirty();
}

void XM_CALLCONV ConstantBufferTyped<TransformationMatrixAndInvTransData>::SetTransformationInvTrans(DirectX::FXMMATRIX new_transformation_inv_trans) {
	DirectX::XMStoreFloat4x4(&buffer_data.transformation_inv_trans, new_transformation_inv_trans);
	SetDirty();
}

void XM_CALLCONV ConstantBufferTyped<ViewProjectionMatrixData>::SetAllTransformations(DirectX::XMMATRIX view_matrix, DirectX::XMMATRIX view_inverse_matrix, DirectX::XMMATRIX view_projection_matrix, DirectX::XMMATRIX orientation_projection_matrix) {
	DirectX::XMStoreFloat4x4(&buffer_data.view_matrix, view_matrix);
	DirectX::XMStoreFloat4x4(&buffer_data.view_inv_matrix, view_inverse_matrix);
	DirectX::XMStoreFloat4x4(&buffer_data.view_proj_matrix, view_projection_matrix);
	DirectX::XMStoreFloat4x4(&buffer_data.orient_proj_matrix, orientation_projection_matrix);
	SetDirty();
}