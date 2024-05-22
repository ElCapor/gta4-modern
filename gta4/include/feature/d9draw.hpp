#ifndef D9DRAW_HPP
#define D9DRAW_HPP
#include <imgui.h>
#include <Windows.h>
#include <d3d9.h>

class d9draw
{
public:
	static bool bDisplay;
	static BOOL bInit;
	static bool bSetPos;

	static HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 D3D9Device);

private:
	static ImVec2 vWindowPos;
	static ImVec2 vWindowSize;

	static void InitImGui(LPDIRECT3DDEVICE9 pDevice);
};

#endif /* D9DRAW_HPP */
