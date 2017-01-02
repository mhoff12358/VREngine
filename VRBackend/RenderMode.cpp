#include "stdafx.h"
#include "RenderMode.h"

ID3D11RenderTargetView* RenderMode::clear_render_target_views[] = {
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

void RenderMode::Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, std::vector<ID3D11RenderTargetView*> rend_tar_views, ID3D11DepthStencilView* dep_sten_view, ID3D11DepthStencilState* dep_sten_state, ID3D11BlendState* bnd_st, ID3D11RasterizerState* rasterizer_state, D3D11_VIEWPORT view) {
	device = dev;
	device_context = dev_con;
	depth_stencil_view = dep_sten_view;
	depth_stencil_state = dep_sten_state;
	blend_state = bnd_st;
	if (rasterizer_state == nullptr) {
		rasterizer_state = GetDefaultRasterizerState(dev);
	}
	rasterizer_state_ = rasterizer_state;
	SetRenderTargetViews(rend_tar_views);
	SetViewport(view);
}

void RenderMode::SetViewport(D3D11_VIEWPORT view) {
	viewports[0] = view;
}

void RenderMode::SetViewport(std::array<int, 2> size, std::array<float, 2> depth_range) {
	SetViewport(size[0], size[1], depth_range[0], depth_range[1]);
}

void RenderMode::SetViewport(std::array<int, 2> offset, std::array<int, 2> size, std::array<float, 2> depth_range) {
	SetViewport(offset[0], offset[1], size[0], size[1], depth_range[0], depth_range[1]);
}

void RenderMode::SetViewport(int width, int height, float min_depth, float max_depth) {
	SetViewport(0, 0, width, height, min_depth, max_depth);
}

void RenderMode::SetViewport(int xoff, int yoff, int width, int height, float min_depth, float max_depth) {
	ZeroMemory(viewports, sizeof(D3D11_VIEWPORT));

	viewports[0].TopLeftX = static_cast<float>(xoff);
	viewports[0].TopLeftY = static_cast<float>(yoff);
	viewports[0].Width = static_cast<float>(width);
	viewports[0].Height = static_cast<float>(height);
	viewports[0].MinDepth = min_depth;
	viewports[0].MaxDepth = max_depth;
}

void RenderMode::SetViewports(const std::vector<D3D11_VIEWPORT> views) {
	num_viewports = min(views.size(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for (int i = 0; i < num_viewports; i++) {
		viewports[i] = views[i];
	}
}

void RenderMode::SetRenderTargetViews(const std::vector<ID3D11RenderTargetView*>& rend_tar_views) {
	num_render_target_views = min(rend_tar_views.size(), D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT);
	for (int i = 0; i < num_render_target_views; i++) {
		render_target_views[i] = rend_tar_views[i];
	}
}

ID3D11RenderTargetView** RenderMode::MutateRenderTargetViews(int num_render_target_views_set) {
	num_render_target_views = num_render_target_views_set;
	return render_target_views;
}

void RenderMode::Cleanup() {
}

void RenderMode::Prepare() {
	device_context->OMSetDepthStencilState(depth_stencil_state, 1);
	device_context->OMSetRenderTargets(num_render_target_views, render_target_views, depth_stencil_view);
	device_context->OMSetBlendState(blend_state, NULL, 0xFFFFFFFF);
	device_context->RSSetViewports(1, viewports);
	device_context->RSSetState(rasterizer_state_);
}

void RenderMode::UnPrepare() {
	UnPrepare(device_context);
}

void RenderMode::UnPrepare(ID3D11DeviceContext* device_context) {
	device_context->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, clear_render_target_views, NULL);
}

void RenderMode::Clear(D3DXCOLOR color) {
	for (int i = 0; i < num_render_target_views; i++) {
		device_context->ClearRenderTargetView(render_target_views[i], color);
	}
}

void RenderMode::ClearDepth() {
	device_context->ClearDepthStencilView(depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 1);
}
	
ID3D11RasterizerState* RenderMode::GetDefaultRasterizerState(ID3D11Device* dev) {
	D3D11_RASTERIZER_DESC raster_desc;
	ZeroMemory(&raster_desc, sizeof(raster_desc));
	raster_desc.DepthClipEnable = true;
	raster_desc.FillMode = D3D11_FILL_SOLID;
	raster_desc.CullMode = D3D11_CULL_BACK;
	ID3D11RasterizerState* raster_state;
	dev->CreateRasterizerState(&raster_desc, &raster_state);
	return raster_state;
}
