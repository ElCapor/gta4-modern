#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"

DWORD WINAPI gta4::MainThread()
{
    Console::Open();
    Console::log("Hello, World!");
    Console::Wait();
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
