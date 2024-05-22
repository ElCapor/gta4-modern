#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
DWORD WINAPI gta4::MainThread()
{
    Console::Open();
    Console::log("Hello, World!");
    CMetaData::init();
    Console::log("Base : ", std::hex, CMetaData::begin());
    d9::hook();
    Console::Wait();
    d9::unhook();
    Console::log("Press any key to exit...");
    Console::Close();
    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)gta4::EjectSelf, 0, 0, 0);
    return 0;
}

DWORD WINAPI gta4::EjectSelf()
{
    Sleep(100);
    FreeLibraryAndExitThread(globals::hDll, 0);
}
