#include "feature/rage/scr/scrThreadHook.hpp"
#include "Windows.h"
#include "detours.h"
#include "utils/pattern.hpp"
#include "console.hpp"

const char*(__thiscall* oGetScriptName)(DWORD*);
const char* __fastcall GetScriptName(DWORD* _this, void* edx);

bool(__stdcall* oRegisterNativeNC)(std::uint32_t a1, std::uint32_t a2);
bool __stdcall hRegisterNativeNC(std::uint32_t a1, std::uint32_t a2);

std::uint32_t(__cdecl* oHashLowerCase)(char*,std::uint32_t);
std::uint32_t __cdecl hHashLowerCase(char* a1, std::uint32_t seed);

void rage::scr::hookScr()
{
    oGetScriptName = (const char*(__thiscall*)(DWORD*))ASLR(0x86F820);
    oRegisterNativeNC = (bool(__stdcall*)(std::uint32_t, std::uint32_t))ASLR(0x86F960);
    oHashLowerCase = (std::uint32_t(__cdecl*)(char*,std::uint32_t))ASLR(0x40BA60);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oGetScriptName, GetScriptName);
    //DetourAttach(&(PVOID&)oRegisterNativeNC, hRegisterNativeNC);
    //DetourAttach(&(PVOID&)oHashLowerCase, hHashLowerCase);
    DetourTransactionCommit();
}

void rage::scr::unhookScr()
{
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
    //DetourDetach(&(PVOID&)oRegisterNativeNC, hRegisterNativeNC);
    DetourDetach(&(PVOID&)::oGetScriptName, GetScriptName);
	DetourTransactionCommit();
}
bool idk = false;
const char *__fastcall GetScriptName(DWORD *_this, void *edx)
{
    if (!idk)
    {
        Console::log("TS ", std::hex, _this);
        rage::scr::GtaThread* thread = reinterpret_cast<rage::scr::GtaThread*>(_this);
        Console::log(thread->scrThread.tExitMsg, "  ", thread->szProgramName);
        idk = true;
    }
    const char* name = oGetScriptName(_this);
    return name;
}

bool __stdcall hRegisterNativeNC(std::uint32_t a1, std::uint32_t a2)
{
    const auto ret = oRegisterNativeNC(a1, a2);
    Console::log("[",__FUNCTION__,"] : ", std::hex, a1, a2);
    return ret;
}

std::uint32_t __cdecl hHashLowerCase(char *a1, std::uint32_t seed)
{
    const auto ret = oHashLowerCase(a1, seed);
    Console::log("[",__FUNCTION__,"] String : ", a1, " seed ", seed, " :result: ", std::hex, ret);
    return ret;
}
