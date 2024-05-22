#ifndef D9HOOK_HPP
#define D9HOOK_HPP
#include <d3d9.h>
#include <cstdint>
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace d9
{
    extern HWND window;
    extern HMODULE hMod;
    extern WNDPROC oWndProc;
    
    typedef HRESULT(APIENTRY* DrawIndexedPrimitive)(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
    HRESULT APIENTRY DrawIndexedPrimitive_hook(LPDIRECT3DDEVICE9, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
    extern std::uint64_t oDrawIndexedPrimitive;

    typedef HRESULT(APIENTRY* EndScene) (LPDIRECT3DDEVICE9);
    HRESULT APIENTRY EndScene_hook(LPDIRECT3DDEVICE9);
    extern std::uint64_t oEndScene;

    typedef HRESULT(APIENTRY* Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
    HRESULT APIENTRY Reset_hook(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
    extern std::uint64_t oReset;

    typedef LRESULT(CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

    void hook();
    void unhook();
}
#endif /* D9HOOK_HPP */
