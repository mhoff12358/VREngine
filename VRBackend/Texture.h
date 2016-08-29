#ifndef __TEXTURE_H_INCLUDED__
#define __TEXTURE_H_INCLUDED__

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#include <array>

#include "Logging.h"
#include "stl.h"

struct TextureUsage {
	TextureUsage(bool is_render_target, bool is_shader_resource, bool is_depth_stencil)
		: is_render_target_(is_render_target), is_shader_resource_(is_shader_resource), is_depth_stencil_(is_depth_stencil){}
	TextureUsage(array<bool, 2> usages) : is_render_target_(usages[0]), is_shader_resource_(usages[1]) {}

	static TextureUsage Rendering() { return TextureUsage(true, true, false); }
	static TextureUsage RenderTarget() { return TextureUsage(true, false, false); }
	static TextureUsage ShaderResource() { return TextureUsage(false, true, false); }
	static TextureUsage DepthStencil() { return TextureUsage(false, true, true); }
	static TextureUsage None() { return TextureUsage(false, false, false); }

	bool is_render_target_;
	bool is_shader_resource_;
	bool is_depth_stencil_;
};

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