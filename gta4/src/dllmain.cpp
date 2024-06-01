#include <Windows.h>
#include <globals.hpp>
#include <gta4.hpp>
#include <imgui/backends/imgui_impl_dx9.h>
#include <imgui/backends/imgui_impl_win32.h>



BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        globals::hDll = hinstDLL;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gta4::MainThread, NULL, 0, NULL);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        gta4::shutdown();
        break;
    }
    return TRUE; // Successful DLL_PROCESS_ATTACH.
}