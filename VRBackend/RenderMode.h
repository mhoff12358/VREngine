#ifndef __RENDER_MODE_H_INCLUDED__
#define __RENDER_MODE_H_INCLUDED__

#include "stdafx.h"

class RenderMode {
public:
	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, vector<ID3D11RenderTargetView*> rend_tar_views, ID3D11DepthStencilView* dep_sten_view, ID3D11DepthStencilState* dep_sten_state, ID3D11BlendState* bnd_st, D3D11_VIEWPORT view);
	void Initialize(ID3D11Device* dev, ID3D11DeviceContext* dev_con, vector<ID3D11RenderTargetView*> rend_tar_views, ID3D11DepthStencilView* dep_sten_view, ID3D11DepthStencilState* dep_sten_state, ID3D11BlendState* bnd_st);
	void SetViewport(D3D11_VIEWPORT view);
	void SetViewport(array<int, 2> offset, array<int, 2> size, array<float, 2> depth_range);
	void SetViewport(array<int, 2> size, array<float, 2> depth_range);
	void SetViewport(int xoff, int yoff, int width, int height, float min_depth, float max_depth);
	void SetViewport(int width, int height, float min_depth, float max_depth);
	void SetViewports(const vector<D3D11_VIEWPORT> views);
	void SetRenderTargetViews(const vector<ID3D11RenderTargetView*>& rend_tar_views);
	ID3D11RenderTargetView** MutateRenderTargetViews(int num_render_target_views_set);

	void Cleanup();

	void Prepare();
	void UnPrepare();
	static void UnPrepare(ID3D11DeviceContext* device_context);

	void Clear(D3DXCOLOR color);
	void ClearDepth();

private:
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;

	int num_render_target_views;
	ID3D11RenderTargetView* render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	ID3D11DepthStencilView* depth_stencil_view;
	ID3D11DepthStencilState* depth_stencil_state;
	ID3D11BlendState* blend_state;
	int num_viewports = 1;
	D3D11_VIEWPORT viewports[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	static ID3D11RenderTargetView* clear_render_target_views[];
};

#endif