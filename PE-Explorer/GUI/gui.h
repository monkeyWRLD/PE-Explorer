#pragma once

#include "../framework.h"
#include "../external/imgui/imgui.h"
#include "../external/imgui/imgui_impl_win32.h"
#include "../external/imgui/imgui_impl_dx11.h"



//D3D11 Stuff	
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;


// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class gui
{
public:

	gui();
	~gui() = default;

public:

	inline float s(float x)
	{
		static auto origY = 1280;
		static auto origX = 800;

		auto curY = ImGui::GetIO().DisplaySize.y;
		auto curX = ImGui::GetIO().DisplaySize.x;

		auto scale = (curY + curX) / (origY + origX);

		return (scale * x);
	};

	void render_handling();
	void render_ui();
	void render_menubar();
	void render_pagebar();


public:

	void open_file();

public:

	
	int page;
};
