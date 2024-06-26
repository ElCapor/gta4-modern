#include "feature/d9draw.hpp"
#include "feature/d9hook.hpp"
#include <imgui/backends/imgui_impl_dx9.h>
#include <imgui/backends/imgui_impl_win32.h>

BOOL d9draw::bInit = FALSE; // Status of the initialization of ImGui.
bool d9draw::bDisplay = true; // Status of the menu display.
bool d9draw::bSetPos = false; // Status to update ImGui window size / position.
ImVec2 d9draw::vWindowPos = { 0, 0 }; // Last ImGui window position.
ImVec2 d9draw::vWindowSize = { 0, 0 }; // Last ImGui window size.
std::vector<d9widget*> d9draw::aWidgets = {};
/**
    @brief : Hook of the EndScene function.
    @param  D3D9Device : Current Direct3D9 Device Object.
    @retval : Result of the original EndScene function.
**/
HRESULT d9draw::hkEndScene(const LPDIRECT3DDEVICE9 D3D9Device)
{
	if (!d9::pDevice)
		d9::pDevice = D3D9Device;

	if (!bInit)
		InitImGui(D3D9Device);

	if (GetAsyncKeyState(VK_DELETE) & 1)
	{
		bDisplay = !bDisplay;
		d9::isMouseWanted = !d9::isMouseWanted;
	}
		

	if (GetAsyncKeyState(VK_F8) & 1)
	{
		d9::UnHookDirectX();
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, d9::hDDLModule, 0, nullptr);
		return d9::oEndScene(D3D9Device);
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (bDisplay)
	{
		for (auto& widget : d9draw::aWidgets)
		{
			if (!widget->IsInit())
			{
				widget->Init();
				widget->IsInit() = true;
			}
			widget->Render(ImGui::GetIO().DeltaTime);
		}
	}

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	return d9::oEndScene(D3D9Device);
}

void d9draw::RegisterWidget(d9widget *widget)
{
	d9draw::aWidgets.push_back(widget);
}
/**
    @brief : function that init ImGui for rendering.
    @param pDevice : Current Direct3D9 Device Object given by the hooked function.
**/
void d9draw::InitImGui(const LPDIRECT3DDEVICE9 pDevice)
{
	D3DDEVICE_CREATION_PARAMETERS CP;
	pDevice->GetCreationParameters(&CP);
	d9::window = CP.hFocusWindow;
	d9::HookWindow();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.Fonts->AddFontDefault();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(d9::window);
	ImGui_ImplDX9_Init(pDevice);

	bInit = TRUE;
}
