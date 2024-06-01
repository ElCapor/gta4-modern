#ifndef D9DRAW_HPP
#define D9DRAW_HPP
#include <imgui.h>
#include <Windows.h>
#include <d3d9.h>
#include <vector>

class d9widget
{
	public:
	virtual void Init() = 0;
	virtual void Render(float dt) = 0;

	bool& IsInit() {return bInit;}
	
	private:
	bool bInit;
};

class d9draw
{
public:
	static bool bDisplay;
	static BOOL bInit;
	static bool bSetPos;

	static HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 D3D9Device);
	static void RegisterWidget(d9widget* widget);

private:
	static ImVec2 vWindowPos;
	static ImVec2 vWindowSize;

	static void InitImGui(LPDIRECT3DDEVICE9 pDevice);

	static std::vector<d9widget*> aWidgets;
};

#endif /* D9DRAW_HPP */
