#ifndef __VIEW_STATE_H_INCLUDED__
#define __VIEW_STATE_H_INCLUDED__

#include "Texture.h"
#include "stl.h"

#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#include <array>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

class InputHandler;

LRESULT CALLBACK WindowProc(HWND window_handler, UINT message, WPARAM wParam, LPARAM lParam);

struct WindowDetails {
	std::array<int, 2> screen_size;
};

class ViewState {
public:
	ViewState(array<int, 2> screen_size) {window_details.screen_size = screen_size;}
	ViewState() : ViewState({800, 600}) {}

	// Set the back_buffer_size to 0, 0 to let it default to the screen size
	void Initialize(InputHandler* input_handler, HINSTANCE ih, int wsc);
	void InitializeWindow();
	void InitializeD3D(std::array<int, 2> back_buffer_size);

	void Cleanup();
	void CleanupD3D();

	ID3D11RenderTargetView* CreateRenderTargetView();

	// Setup/config values
	HINSTANCE instance_handler;
	int window_show_command;
	WindowDetails window_details;

	// Created objects
	HWND window_handler;

	// D3D objects
	IDXGISwapChain* swap_chain;
	ID3D11Device* device_interface;
	ID3D11DeviceContext* device_context;
	ID3D11Texture2D* back_buffer_texture;
};

#endif