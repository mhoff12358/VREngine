#pragma once

#include "stdafx.h"

#include "ShaderStages.h"

#define PER_FRAME_CONSTANT_BUFFER_REGISTER 0
#define PER_MODEL_CONSTANT_BUFFER_REGISTER 1
// Per batch constant buffers can use registers 2+
#define PER_BATCH_CONSTANT_BUFFER_REGISTER 2

struct TransformationMatrixData {
	DirectX::XMFLOAT4X4 transformation;
};

struct ViewProjectionMatrixData {
	DirectX::XMFLOAT4X4 view_matrix;
	DirectX::XMFLOAT4X4 view_proj_matrix;
	DirectX::XMFLOAT4X4 orient_proj_matrix;
};

struct TransformationMatrixAndInvTransData {
	DirectX::XMFLOAT4X4 transformation;
	DirectX::XMFLOAT4X4 transformation_inv_trans;
};

class ConstantBuffer
{
public:
	explicit ConstantBuffer(ShaderStages stages);
	~ConstantBuffer();

	void CreateBuffer(ID3D11Device* device_interface);
	void PushBuffer(ID3D11DeviceContext* device_context);
	void SetDirty();
	void ForcePushBuffer(ID3D11DeviceContext* device_context);

	virtual void* EditBufferData() = 0;
	virtual void* EditBufferData(bool set_dirty) = 0;
	virtual const void* ReadBufferData() = 0;
	virtual unsigned int GetBufferDataSize() = 0;

	void Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context, int buffer_register) const;

protected:
	ID3D11Buffer* const_buffer;
	ShaderStages pipeline_stages;
	bool dirty;
};

class SizedConstantBuffer : public ConstantBuffer
{
public:
	SizedConstantBuffer(ShaderStages stages, unsigned int size);
	~SizedConstantBuffer();

	virtual void* EditBufferData() override;
	virtual void* EditBufferData(bool set_dirty) override;
	virtual const void* ReadBufferData() override;
	virtual unsigned int GetBufferDataSize() override;

private:
	vector<char> buffer_data;
};

template <typename ConstantBufferData>
class ConstantBufferTypedTemp : public ConstantBuffer {
public:
	ConstantBufferTypedTemp(ShaderStages stages) : ConstantBuffer(stages) {}
	ConstantBufferTypedTemp(ConstantBufferTypedTemp&& other)
		: ConstantBuffer(other.pipeline_stages), buffer_data(std::move(other.buffer_data))
	{
		const_buffer = other.const_buffer;
		dirty = other.dirty;
		other.const_buffer = NULL;
		other.dirty = false;
	}

	void* EditBufferData();
	void* EditBufferData(bool set_dirty);
	const void* ReadBufferData();
	const ConstantBufferData* ReadBufferDataTyped();
	unsigned int GetBufferDataSize();

	ConstantBufferData& EditBufferDataRef();
	ConstantBufferData& EditBufferDataRef(bool set_dirty);
	const ConstantBufferData& ReadBufferDataRef();

	void SetBufferData(const ConstantBufferData& new_data);

protected:
	ConstantBufferData buffer_data;
};

template <typename ConstantBufferData>
unsigned int ConstantBufferTypedTemp<ConstantBufferData>::GetBufferDataSize() {
	return (((sizeof(ConstantBufferData)-1) / 16) + 1) * 16;
}

template <typename ConstantBufferData>
void* ConstantBufferTypedTemp<ConstantBufferData>::EditBufferData() {
	SetDirty();
	return (void*)&buffer_data;
}

template <typename ConstantBufferData>
void* ConstantBufferTypedTemp<ConstantBufferData>::EditBufferData(bool set_dirty) {
	if (set_dirty) {
		SetDirty();
	}
	return (void*)&buffer_data;
}

template <typename ConstantBufferData>
const void* ConstantBufferTypedTemp<ConstantBufferData>::ReadBufferData() {
	return (void*)&buffer_data;
}

template <typename ConstantBufferData>
const ConstantBufferData* ConstantBufferTypedTemp<ConstantBufferData>::ReadBufferDataTyped() {
	return &buffer_data;
}

template <typename ConstantBufferData>
ConstantBufferData& ConstantBufferTypedTemp<ConstantBufferData>::EditBufferDataRef() {
	SetDirty();
	return buffer_data;
}

template <typename ConstantBufferData>
ConstantBufferData& ConstantBufferTypedTemp<ConstantBufferData>::EditBufferDataRef(bool set_dirty) {
	if (set_dirty) {
		SetDirty();
	}
	return buffer_data;
}

template <typename ConstantBufferData>
const ConstantBufferData& ConstantBufferTypedTemp<ConstantBufferData>::ReadBufferDataRef() {
	return &buffer_data;
}

template <typename ConstantBufferData>
void ConstantBufferTypedTemp<ConstantBufferData>::SetBufferData(const ConstantBufferData& new_data) {
	SetDirty();
	buffer_data = new_data;
}

template <typename ConstantBufferData>
class ConstantBufferTyped : public ConstantBufferTypedTemp < ConstantBufferData > {

};

template <>
class ConstantBufferTyped<TransformationMatrixData> : public ConstantBufferTypedTemp<TransformationMatrixData>{
public:
	ConstantBufferTyped(ShaderStages stages) : ConstantBufferTypedTemp(stages) {}

	void XM_CALLCONV SetTransformation(DirectX::FXMMATRIX new_transformation);
};

template <>
class ConstantBufferTyped<ViewProjectionMatrixData> : public ConstantBufferTypedTemp<ViewProjectionMatrixData>{
public:
	ConstantBufferTyped(ShaderStages stages) : ConstantBufferTypedTemp(stages) {}

	void XM_CALLCONV SetAllTransformations(DirectX::XMMATRIX view_matrix, DirectX::XMMATRIX view_projection_matrix, DirectX::XMMATRIX orientation_projection_matrix);
};

template <>
class ConstantBufferTyped<TransformationMatrixAndInvTransData> : public ConstantBufferTypedTemp<TransformationMatrixAndInvTransData>{
public:
	ConstantBufferTyped(ShaderStages stages) : ConstantBufferTypedTemp(stages) {}

	void XM_CALLCONV SetBothTransformations(DirectX::FXMMATRIX new_transformation);
	void XM_CALLCONV SetTransformation(DirectX::FXMMATRIX new_transformation);
	void XM_CALLCONV SetTransformationInvTrans(DirectX::FXMMATRIX new_transformation_inv_trans);
};