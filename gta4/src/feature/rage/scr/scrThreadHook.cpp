#include "feature/rage/scr/scrThreadHook.hpp"
#include "Windows.h"
#include "detours.h"
#include "utils/pattern.hpp"
#include "console.hpp"

const char*(__thiscall* oGetScriptName)(DWORD*);
const char* __fastcall GetScriptName(DWORD* _this, void* edx);

void rage::scr::hookScr()
{
    oGetScriptName = (const char*(__thiscall*)(DWORD*))ASLR(0x86F820);
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)oGetScriptName, GetScriptName);
    DetourTransactionCommit();
}

void rage::scr::unhookScr()
{
    DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
    DetourDetach(&(PVOID&)::oGetScriptName, GetScriptName);
	DetourTransactionCommit();
}
const char *__fastcall GetScriptName(DWORD *_this, void *edx)
{
    const char* name = oGetScriptName(_this);
    Console::log("Get name ", name);
    return name;
}