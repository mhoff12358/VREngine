#pragma once
#include "stdafx.h"

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
