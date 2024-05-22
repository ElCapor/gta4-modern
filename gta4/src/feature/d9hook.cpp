#include "feature/d9hook.hpp"
#include "console.hpp"
#include "polyhook2/Detour/x86Detour.hpp"

#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"

HWND d9::window = 0;
HMODULE d9::hMod = 0;
d9::WNDPROC d9::oWndProc = 0;
std::uint64_t d9::oDrawIndexedPrimitive = 0;
std::uint64_t d9::oEndScene = 0;
std::uint64_t d9::oReset = 0;

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	return CallWindowProc(d9::oWndProc, hWnd, uMsg, wParam, lParam);
}

PLH::x86Detour* EndSceneHook;
bool d9init()
{
    LPDIRECT3D9 d3d = NULL;
    LPDIRECT3DDEVICE9 d3ddev = NULL;
    HWND tmpWnd = CreateWindowA("BUTTON", "Temp Window", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 300, NULL, NULL, d9::hMod, NULL);
	if (tmpWnd == NULL)
	{
        Console::error("Failed to create window...");
		return false;
	}

	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
        Console::error("Failed to create d9 device...");
		DestroyWindow(tmpWnd);
		return false;
	}

    D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = tmpWnd;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	HRESULT result = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, tmpWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (result != D3D_OK)
	{
        Console::error("CreateDevice did not return D3D_OK, aborting...");
		d3d->Release();
		DestroyWindow(tmpWnd);
		return 0;
	}

    DWORD* dVtable = (DWORD*)d3ddev;
	dVtable = (DWORD*)dVtable[0];

    EndSceneHook = new PLH::x86Detour(static_cast<std::uint64_t>(dVtable[42]), (std::uint64_t)&d9::EndScene_hook, &d9::oEndScene);
    EndSceneHook->hook();

    d9::oWndProc = (d9::WNDPROC)SetWindowLongPtr(FindWindowA(NULL, "GTAIV"), GWLP_WNDPROC, (LONG_PTR)WndProc);
    d3ddev->Release();
	d3d->Release();
	DestroyWindow(tmpWnd);
    return true;
}

void d9::hook()
{
    if (d9init())
        Console::log("Success DX9 Hook");
}

void d9::unhook()
{
    EndSceneHook->unHook();
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
    
}




HRESULT APIENTRY d9::EndScene_hook(LPDIRECT3DDEVICE9 device)
{
    static bool init = true;
	if (init)
	{
		init = false;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui_ImplWin32_Init(FindWindowA(NULL, "GTAIV"));

		ImGui_ImplDX9_Init(device);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();
	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    return PLH::FnCast(oEndScene, EndScene())(device);
}
HRESULT APIENTRY d9::Reset_hook(LPDIRECT3DDEVICE9 device, D3DPRESENT_PARAMETERS * params)
{
    return E_NOTIMPL;
}
HRESULT APIENTRY d9::DrawIndexedPrimitive_hook(LPDIRECT3DDEVICE9 pD3D9, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
    return E_NOTIMPL;
}