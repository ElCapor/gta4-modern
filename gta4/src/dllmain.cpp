#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <globals.hpp>
#include <gta4.hpp>



BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        globals::hDll = hinstDLL;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)gta4::MainThread, NULL, 0, NULL);
        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
        }

        break;
    }
    return TRUE; // Successful DLL_PROCESS_ATTACH.
}