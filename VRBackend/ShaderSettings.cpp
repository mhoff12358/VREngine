#include "ShaderSettings.h"

ShaderSettings::ShaderSettings()
	: constant_buffer(NULL) {

}

ShaderSettings::ShaderSettings(ConstantBuffer* buffer)
	: constant_buffer(buffer)
{
}


ShaderSettings::~ShaderSettings()
{
}

void ShaderSettings::Prepare(ID3D11Device* device, ID3D11DeviceContext* device_context) const {
	if (!IsDummy()) {
		constant_buffer->PushBuffer(device_context);
		constant_buffer->Prepare(device, device_context, buffer_register);
	}
}

bool ShaderSettings::IsDummy() const {
	return constant_buffer == NULL;
}