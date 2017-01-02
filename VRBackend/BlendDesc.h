#pragma once

#include "stdafx.h"

class BlendDesc {
public:
	static void Init();

	static D3D11_BLEND_DESC no_alpha_blend_state_desc;
	static D3D11_BLEND_DESC keep_new_alpha_blend_state_desc;
	static D3D11_BLEND_DESC keep_new_alpha_with_alpha_blend_state_desc;
	static D3D11_BLEND_DESC drop_alpha_with_alpha_blend_state_desc;
	static D3D11_BLEND_DESC additative_for_all_blend_state_desc;
	static D3D11_DEPTH_STENCIL_DESC keep_nearer_depth_test;
};