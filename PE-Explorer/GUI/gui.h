#pragma once

#include <d3d9.h>



namespace GUI {
	// constant window size
	constexpr int WIDTH = 1000;
	constexpr int HEIGHT = 800;
	
	

	//exit check
	inline bool isRunning = true;

	// winAPI window variables
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };

	//inline points for window movement
	inline POINTS position  = { };

	// directx 9 variables
	inline PDIRECT3D9 d3dptr = nullptr;
	inline PDIRECT3DDEVICE9 d3ddev = nullptr;
	inline D3DPRESENT_PARAMETERS d3dpp = { };

	//window creation and destruction
	void CreateHWindow(const char* windowName) noexcept;
	void DestroyHWindow() noexcept;

	//device creation and destruction
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	// ImGUI creation and destruction
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;

	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
} 