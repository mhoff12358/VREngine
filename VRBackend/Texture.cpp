#include "stdafx.h"
#include "Texture.h"

void Texture::Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, std::array<int, 2> new_size, const D3D11_TEXTURE2D_DESC* texture_desc) {
	device_interface = dev;
	device_context = dev_con;
	Cleanup();

	size = new_size;

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
	ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
	if (texture_desc != NULL) {
		render_target_view_desc.Format = texture_desc->Format;
	} else {
		render_target_view_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	}
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	render_target_view_desc.Texture2D.MipSlice = 0;

	CreateTexture(texture_desc);
	CreateResources(&render_target_view_desc);
}

void Texture::Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, std::array<int, 2> new_size, ID3D11Texture2D* existing_texture, D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc) {
	device_interface = dev;
	device_context = dev_con;
	Cleanup();

	size = new_size;

	texture = existing_texture;

	CreateResources(rtv_desc);
}

void Texture::Cleanup() {
	if (render_target_view != NULL) {
		render_target_view->Release();
	}
	if (shader_resource_view != NULL) {
		shader_resource_view->Release();
	}
	if (sampler != NULL) {
		sampler->Release();
	}
}

void Texture::CreateResources(D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc) {
	if (usage_.is_shader_resource_) {
		CreateShaderResourceView();
		CreateSampler();
	}
	if (usage_.is_render_target_) {
		CreateRenderTargetView(rtv_desc);
	}
	if (usage_.is_depth_stencil_) {
		CreateDepthStencilView();
	}
}

D3D11_TEXTURE2D_DESC Texture::GetDefaultTextureDesc() {
	D3D11_TEXTURE2D_DESC texture_desc;
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	texture_desc.Width = size[0];
	texture_desc.Height = size[1];
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1;
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texture_desc.SampleDesc = { 1, 0 }; // Count, quality
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	if (!usage_.is_render_target_ && usage_.is_shader_resource_) {
		texture_desc.Usage = D3D11_USAGE_DYNAMIC;
	}
	texture_desc.BindFlags = 0;
	if (usage_.is_shader_resource_) { texture_desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE; }
	if (usage_.is_render_target_) { texture_desc.BindFlags |= D3D11_BIND_RENDER_TARGET; }
	texture_desc.CPUAccessFlags = 0;
	if (!usage_.is_render_target_ && usage_.is_shader_resource_) {
		texture_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	texture_desc.MiscFlags = 0;
	return texture_desc;
}

D3D11_TEXTURE2D_DESC DepthTexture::GetDefaultDepthTextureDesc() {
	D3D11_TEXTURE2D_DESC depth_texture_desc;
	ZeroMemory(&depth_texture_desc, sizeof(depth_texture_desc));
	depth_texture_desc.Width = 0;
	depth_texture_desc.Height = 0;
	depth_texture_desc.MipLevels = 1;
	depth_texture_desc.ArraySize = 1;
	depth_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_texture_desc.SampleDesc = { 1, 0 }; // Count, quality
	depth_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_texture_desc.CPUAccessFlags = 0;
	depth_texture_desc.MiscFlags = 0;
	return depth_texture_desc;
}

D3D11_TEXTURE2D_DESC Texture::GetDesc() const {
	D3D11_TEXTURE2D_DESC return_desc;
	texture->GetDesc(&return_desc);
	return return_desc;
}

void Texture::CreateTexture(const D3D11_TEXTURE2D_DESC* texture_desc) {
	D3D11_TEXTURE2D_DESC local_tex_desc;
	if (texture_desc == NULL) {
		local_tex_desc = GetDefaultTextureDesc();
		texture_desc = &local_tex_desc;
	}
	// Currently provides no initial data to the texture
	// The texture that is created is stored in the texture attribute
	device_interface->CreateTexture2D(texture_desc, NULL, &texture);
	owns_texture = true;
}

void DepthTexture::CreateTexture(const D3D11_TEXTURE2D_DESC* texture_desc) {
	D3D11_TEXTURE2D_DESC local_tex_desc;
	if (texture_desc == NULL) {
		local_tex_desc = GetDefaultDepthTextureDesc();
		local_tex_desc.Width = size[0];
		local_tex_desc.Height = size[1];
		texture_desc = &local_tex_desc;
	}
	device_interface->CreateTexture2D(texture_desc, NULL, &texture);
}

void Texture::CreateShaderResourceView() {
	D3D11_TEXTURE2D_DESC tex_desc;
	texture->GetDesc(&tex_desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC shad_res_view_desc;
	ZeroMemory(&shad_res_view_desc, sizeof(shad_res_view_desc));
	shad_res_view_desc.Format = MapShaderResourceFormat(tex_desc.Format);
	shad_res_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shad_res_view_desc.Texture2D.MostDetailedMip = 0;
	shad_res_view_desc.Texture2D.MipLevels = 1;

	device_interface->CreateShaderResourceView(texture, &shad_res_view_desc, &shader_resource_view);
}

void Texture::CreateSampler() {
	D3D11_SAMPLER_DESC samp_desc;
	ZeroMemory(&samp_desc, sizeof(samp_desc));
	//samp_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samp_desc.Filter = D3D11_FILTER_ANISOTROPIC;
	samp_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samp_desc.MipLODBias = 0;
	samp_desc.MaxAnisotropy = 1; // Shouldn't be needed because not using anisotropy
	samp_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samp_desc.MinLOD = 0;
	samp_desc.MaxLOD = 0;

	device_interface->CreateSamplerState(&samp_desc, &sampler);
}

void Texture::CreateRenderTargetView(D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc) {
	device_interface->CreateRenderTargetView(texture, rtv_desc, &render_target_view);
}

void Texture::CreateDepthStencilView() {
	D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
	D3D11_TEXTURE2D_DESC texture_desc;
	texture->GetDesc(&texture_desc);
	depth_stencil_view_desc.Format = MapDepthStencilFormat(texture_desc.Format);
	depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_view_desc.Flags = 0;
	depth_stencil_view_desc.Texture2D.MipSlice = 0;

	device_interface->CreateDepthStencilView(texture, &depth_stencil_view_desc, &depth_stencil_view);
}

void Texture::SetAsResource(int texture_number, int sampler_number) {
	device_context->PSSetShaderResources(texture_number, 1, &shader_resource_view);
	device_context->PSSetSamplers(sampler_number, 1, &sampler);
}

void Texture::UnsetAsResource(int texture_number, int sampler_number) const {
	void* null_resource = NULL;
	device_context->PSSetShaderResources(texture_number, 1, (ID3D11ShaderResourceView**)&null_resource);
	device_context->PSSetSamplers(sampler_number, 1, (ID3D11SamplerState**)&null_resource);
}

void Texture::Fill() {
	D3D11_MAPPED_SUBRESOURCE ms;
	device_context->Map(texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
	byte* data = new byte[ms.RowPitch * 128];
	for (int row = 0; row < 128; row++) {
		float* row_ptr = (float*)(data + ms.RowPitch*row);
		for (int i = 0; i < 128 * 4; i += 4) {
			row_ptr[i + 0] = ((float)i)/(128.0f*4.0f);
			row_ptr[i + 1] = 0.0f;
			row_ptr[i + 2] = 1.0f;
			row_ptr[i + 3] = 1.0f;
		}
	}
	memcpy(ms.pData, data, ms.RowPitch*128);
	delete[] data;
	device_context->Unmap(texture, 0);
}

void Texture::CopyFrom(ID3D11Texture2D* other_texture) {
	ID3D11Texture2D* a = GetTexture();
	device_context->CopyResource(a, other_texture);
}

void Texture::CopyFrom(const Texture& other_texture) {
	CopyFrom(other_texture.GetTexture());
}

void Texture::CopyFrom(ID3D11Texture2D* other_texture, D3D11_VIEWPORT dest_viewport) {
	device_context->CopySubresourceRegion(GetTexture(), 0, static_cast<unsigned int>(dest_viewport.TopLeftX), static_cast<unsigned int>(dest_viewport.TopLeftY), 0, other_texture, 0, NULL);
}

void Texture::CopyFrom(const Texture& other_texture, D3D11_VIEWPORT dest_viewport) {
	CopyFrom(other_texture.GetTexture(), dest_viewport);
}

vector<float> Texture::CopyOutData() {
	D3D11_TEXTURE2D_DESC tex_desc;
	texture->GetDesc(&tex_desc);

	ID3D11Texture2D* texture_to_use = texture;
	if (!(tex_desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ)) {
		tex_desc.Usage = D3D11_USAGE_STAGING;
		tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		tex_desc.BindFlags = 0;
		tex_desc.MipLevels = 1;
		tex_desc.MiscFlags = 0;
		tex_desc.SampleDesc = { 1, 0 };
		tex_desc.ArraySize = 1;
		
		device_interface->CreateTexture2D(&tex_desc, NULL, &texture_to_use);
		device_context->CopySubresourceRegion(texture_to_use, 0, 0, 0, 0, texture, 0, NULL);
		//new_texture_to_use->CopyFrom(*this);
		D3D11_BOX box;
		box.left = 0;
		box.right = 1;
		box.top = 0;
		box.bottom = 1;
		box.front = 0;
		box.back = 1;
		//device_context->CopySubresourceRegion(new_texture_to_use->texture, 0, 0, 0, 0, texture, 0, &box);
		//texture_to_use = new_texture_to_use;
	}


	D3D11_MAPPED_SUBRESOURCE ms;
	device_context->Map(texture_to_use, 0, D3D11_MAP_READ, 0, &ms);
	vector<float> result_floats(size[0] * size[1] * 4);
	char* result_buffer = (char*)result_floats.data();
	for (int row = 0; row < size[1]; row++) {
		if (tex_desc.Format == DXGI_FORMAT_R8G8B8A8_UNORM) {
			for (int col = 0; col < size[0]; col++) {
				for (int color = 0; color < 4; color++) {
					unsigned char a = *(((unsigned char*)ms.pData) + row * ms.RowPitch + col * 4 + color);
					result_floats[row * size[0] * 4 + col * 4 + color] = *(((unsigned char*)ms.pData) + row * ms.RowPitch + col * 4 + color) / 255.0f;
				}
			}
		} else if (tex_desc.Format == DXGI_FORMAT_R32G32B32A32_FLOAT) {
			memcpy(result_buffer, (char*)ms.pData + ms.RowPitch * row, sizeof(float) * 4 * size[0]);
			result_buffer += sizeof(float) * 4 * size[0];
		}
	}
	device_context->Unmap(texture_to_use, 0);

	if (texture_to_use != texture) {
		texture_to_use->Release();
	}
	return result_floats;
}

ID3D11Texture2D* Texture::GetTexture() const {
	return texture;
}

ID3D11RenderTargetView* Texture::GetRenderTargetView() const {
	return render_target_view;
}

ID3D11ShaderResourceView* Texture::GetShaderResourceView() const {
	return shader_resource_view;
}

ID3D11SamplerState* Texture::GetSampler() const {
	return sampler;
}

ID3D11DepthStencilView* Texture::GetDepthStencilView() const {
	return depth_stencil_view;
}

bool Texture::GetIsShaderResource() const {
	return usage_.is_shader_resource_;
}

bool Texture::GetIsRenderTarget() const {
	return usage_.is_render_target_;
}

bool Texture::GetIsDepthStencil() const {
	return usage_.is_depth_stencil_;
}

std::array<int, 2> Texture::GetSize() const {
	return size;
}

DXGI_FORMAT Texture::MapShaderResourceFormat(DXGI_FORMAT input_format) {
	switch (input_format) {
	case DXGI_FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_R32_FLOAT;
	default:
		return input_format;
	}
}

DXGI_FORMAT Texture::MapDepthStencilFormat(DXGI_FORMAT input_format) {
	switch (input_format) {
	case DXGI_FORMAT_R32_TYPELESS:
		return DXGI_FORMAT_D32_FLOAT;
	default:
		return input_format;
	}
}