#include "gui.h"
#include "../main.h"
#include "../SDK/GrabPEInfo.h"
#include "../external/IMGUI/imgui.h"
#include "../external/IMGUI/imgui_impl_dx9.h"
#include "../external/IMGUI/imgui_impl_win32.h"



	extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wParam, LPARAM lParam);

	LRESULT WindowProcess(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {

		if (ImGui_ImplWin32_WndProcHandler(window, message, wParam, lParam))
			return true;

		switch (message) 
		{

		case WM_SIZE:
			if (GUI::d3ddev != NULL && wParam != SIZE_MINIMIZED)
			{
				GUI::d3dpp.BackBufferWidth = LOWORD(lParam);
				GUI::d3dpp.BackBufferHeight = HIWORD(lParam);
				GUI::ResetDevice();
			}
			return 0;

		case WM_LBUTTONDOWN: {
			GUI::position = MAKEPOINTS(lParam); // set click points
		}return 0;

		case WM_MOUSEMOVE: {
			if (wParam == MK_LBUTTON)
			{
				const auto points = MAKEPOINTS(lParam);
				auto rect = ::RECT{ };

				GetWindowRect(GUI::window, &rect);

				rect.left += points.x - GUI::position.x;
				rect.top += points.y - GUI::position.y;

				if (GUI::position.x >= 0 &&
					GUI::position.x <= GUI::WIDTH &&
					GUI::position.y >= 0 && GUI::position.y <= 19)
					SetWindowPos(
						GUI::window,
						HWND_TOPMOST,
						rect.left,
						rect.top,
						0, 0,
						SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
					);
			}

		}return 0;

		case WM_DESTROY: {
			PostQuitMessage(0);
		}return 0;

		case WM_SYSCOMMAND: {
			if ((wParam & 0xfff0) == SC_KEYMENU)
				return 0;
		}break;

		}

		return DefWindowProcW(window, message, wParam, lParam);
	}

	void GUI::CreateHWindow(const char* windowName) noexcept {

		
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.style = CS_HREDRAW | CS_VREDRAW;
		windowClass.lpfnWndProc = WindowProcess;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = GetModuleHandleA(0);
		windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		windowClass.hbrBackground = 0;
		windowClass.lpszMenuName = 0;
		windowClass.lpszClassName = L"PE GUI";
		windowClass.hIconSm = 0;

		RegisterClassEx(&windowClass);

		window = CreateWindowEx(
			0,
			L"PE GUI",
			L"PE GUI",
			WS_POPUPWINDOW,
			0,
			0,
			WIDTH,
			HEIGHT,
			0,
			0,
			windowClass.hInstance,
			0
		);

		ShowWindow(window, SW_SHOWDEFAULT);
		UpdateWindow(window);
	}

	void GUI::DestroyHWindow() noexcept {
		DestroyWindow(window);
		UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
	}


	bool GUI::CreateDevice() noexcept {

		d3dptr = Direct3DCreate9(D3D_SDK_VERSION);

		if (!d3dptr)
			return false;

		ZeroMemory(&d3dpp, sizeof(d3dpp));

		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		if (d3dptr->CreateDevice(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			window,
			D3DCREATE_HARDWARE_VERTEXPROCESSING,
			&d3dpp,
			&d3ddev) < 0)
			return false;

		return true;
	}

	void GUI::ResetDevice() noexcept {

		ImGui_ImplDX9_InvalidateDeviceObjects();

		const auto result = d3ddev->Reset(&d3dpp);

		if (result == D3DERR_INVALIDCALL)
			IM_ASSERT(0);

		ImGui_ImplDX9_CreateDeviceObjects();
	}

	void GUI::DestroyDevice() noexcept {
		if (d3ddev)
		{
			d3ddev->Release();
			d3ddev = nullptr;
		}

		if (d3dptr)
		{
			d3dptr->Release();
			d3dptr = nullptr;
		}
	}



	void GUI::CreateImGui() noexcept {

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ::ImGui::GetIO();

		io.IniFilename = NULL;;

		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style->FramePadding = ImVec2(8, 6);

		style->Colors[ImGuiCol_TitleBg] = ImColor(92, 92, 92, 92);
		style->Colors[ImGuiCol_TitleBgActive] = ImColor(92, 92, 92, 92);
		style->Colors[ImGuiCol_WindowBg] = ImColor(31, 31, 31, 92);

		style->Colors[ImGuiCol_Button] = ImColor(236, 255, 0, 180);
		style->Colors[ImGuiCol_ButtonHovered] = ImColor(236, 255, 0, 200);
		style->Colors[ImGuiCol_ButtonActive] = ImColor(236, 255, 0, 200);

		style->Colors[ImGuiCol_Header] = ImColor(236, 255, 0, 180);
		style->Colors[ImGuiCol_HeaderHovered] = ImColor(236, 255, 0, 200);
		

		static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

		ImGui_ImplWin32_Init(window);
		ImGui_ImplDX9_Init(d3ddev);
	}
	void GUI::DestroyImGui() noexcept {
		ImGui_ImplDX9_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}



	void GUI::BeginRender() noexcept {
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT)
			{
				isRunning = !isRunning;
				return;
			}
		}

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}
	void GUI::EndRender() noexcept {
		ImGui::EndFrame();

		d3ddev->SetRenderState(D3DRS_ZENABLE, FALSE);
		d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		d3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		d3ddev->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

		if (d3ddev->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			d3ddev->EndScene();
		}

		const auto result = d3ddev->Present(0, 0, 0, 0);

		// Handle loss of D3D9 device
		if (result == D3DERR_DEVICELOST && d3ddev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();
	}
	void GUI::Render() noexcept {

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ 1000, 800 });
		
		ImGui::Begin(
			"PE-Explorer",
			&isRunning,
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse|
			ImGuiWindowFlags_NoBringToFrontOnFocus
		);

		if (ImGui::BeginMainMenuBar())
		{
			ImGui::MenuItem("PE-Explorer", NULL, false, false);
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O")) { SDK::PE::Main(); }
				if (ImGui::MenuItem("Close", "Alt+F4")) { ExitPrgm(); }
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
		

		ImGui::Text("APPLICATION NAME.EXTENTION");

		ImGui::Columns(2, "Columns", false);
		ImGui::SetColumnOffset(1, 210);

		ImGui::BeginChild("leftTab", ImVec2(200, (HEIGHT - 60)), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);


		



		if (ImGui::Button("Summary", ImVec2(200, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::Button("Sections", ImVec2(180, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::Button("Directories", ImVec2(180, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::Button("Imports", ImVec2(180, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::Button("Exports", ImVec2(180, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::Button("Resources", ImVec2(180, 50))) {

		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Headers"))
		{
			static int selected = -1;

			if (ImGui::Selectable("DOS Header", selected == 1)) {
				selected = 1;
			}
			if (ImGui::Selectable("NT Header", selected == 2)) {
				selected = 2;
			}
			if (ImGui::Selectable("File Header", selected == 3)) {
				selected = 3;
			}
			if (ImGui::Selectable("Optional Header", selected == 4)) {
				selected = 4;
			}
			if (ImGui::Selectable("Data Header", selected == 5)) {
				selected = 5;
			}

		}
		ImGui::EndChild();
		ImGui::NextColumn();


		ImGui::BeginChild("rightTab", ImVec2(770, (HEIGHT - 60)), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::Text("TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST TEST ");


		ImGui::ShowDemoWindow();

		ImGui::EndChild();
		ImGui::End();
	}
