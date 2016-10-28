#ifndef __TEXTURE_H_INCLUDED__
#define __TEXTURE_H_INCLUDED__

#include "stdafx.h"

#include "TextureUsage.h"

class Texture {
public:
	Texture(TextureUsage usage) :
		usage_(usage), texture(NULL),
		shader_resource_view(NULL), sampler(NULL),
		render_target_view(NULL), depth_stencil_view(NULL)
	{}
	Texture() : Texture(TextureUsage::None()) {}

	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, std::array<int, 2> size, const D3D11_TEXTURE2D_DESC* texture_desc = NULL);
	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, std::array<int, 2> size, ID3D11Texture2D* existing_texture, D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc = NULL);

	void Cleanup();

	void SetAsResource(int texture_number, int sampler_number);
	void UnsetAsResource(int texture_number, int sampler_number) const;
	void Fill();

	ID3D11Texture2D* GetTexture() const;
	ID3D11RenderTargetView* GetRenderTargetView() const;
	ID3D11ShaderResourceView* GetShaderResourceView() const;
	ID3D11SamplerState* GetSampler() const;
	ID3D11DepthStencilView* GetDepthStencilView() const;

	void CopyFrom(ID3D11Texture2D* other_texture);
	void CopyFrom(const Texture& other_texture);
	void CopyFrom(ID3D11Texture2D* other_texture, D3D11_VIEWPORT dest_viewport);
	void CopyFrom(const Texture& other_texture, D3D11_VIEWPORT dest_viewport);

	bool GetIsShaderResource() const;
	bool GetIsRenderTarget() const;
	bool GetIsDepthStencil() const;

	vector<float> CopyOutData();

	std::array<int, 2> GetSize() const;
	D3D11_TEXTURE2D_DESC GetDesc() const;

protected:
	virtual void CreateResources(D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc);
	virtual void CreateTexture(const D3D11_TEXTURE2D_DESC* texture_desc);
	virtual void CreateShaderResourceView();
	virtual void CreateSampler();
	virtual void CreateRenderTargetView(D3D11_RENDER_TARGET_VIEW_DESC* rtv_desc);
	virtual void CreateDepthStencilView();

	std::array<int, 2> size;
	bool owns_texture = false;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* shader_resource_view;
	ID3D11SamplerState* sampler;
	ID3D11RenderTargetView* render_target_view;
	ID3D11DepthStencilView* depth_stencil_view;

	// Things injected
	ID3D11Device* device_interface;
	ID3D11DeviceContext* device_context;

	// Functionality state, shouldn't change once initalized
	TextureUsage usage_;

private:
	D3D11_TEXTURE2D_DESC GetDefaultTextureDesc();

	static DXGI_FORMAT MapShaderResourceFormat(DXGI_FORMAT input_format);
	static DXGI_FORMAT MapDepthStencilFormat(DXGI_FORMAT input_format);
};

class DepthTexture : public Texture {
public:
	DepthTexture() : Texture(TextureUsage::DepthStencil()) {}
	virtual void CreateTexture(const D3D11_TEXTURE2D_DESC* texture_desc) override;

	static D3D11_TEXTURE2D_DESC GetDefaultDepthTextureDesc();
};

#endif