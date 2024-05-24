#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
#include "feature/rage/scr/scrThreadHook.hpp"
#define NO_UI
DWORD WINAPI gta4::MainThread()
{
    Console::Open();
    Console::log("Hello, World!");
    CMetaData::init();
    Console::log("Base : ", std::hex, CMetaData::begin());
    #ifndef NO_UI
    d9::hDDLModule = globals::hDll;
    d9::HookDirectX();
    #endif
    rage::scr::hookScr();
    Console::Wait();
    Console::log("Press any key to exit...");
    Console::Close();
    //CreateThread(0, 0, (LPTHREAD_START_ROUTINE)gta4::EjectSelf, 0, 0, 0);
    return 0;
}

DWORD WINAPI gta4::EjectSelf()
{
    Sleep(5000);
    FreeLibraryAndExitThread(globals::hDll, 0);
}

void gta4::shutdown()
{
    rage::scr::unhookScr();
    #ifndef NO_UI
    d9::UnHookDirectX();
    #endif
}