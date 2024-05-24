#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
#include "feature/rage/scr/scrThreadHook.hpp"

#define NO_UI
#define FEATURE_FIX_IV_SDK

#ifdef FEATURE_FIX_IV_SDK

#define PATTERN pHolder
// pattern holder
struct pHolder
{
public:
    const char* aob;//ida format aob
    void* target; // target address

    pHolder(const char* aob = "", void* target = nullptr) : aob(aob), target(target)
    {
        // init the structure
    }

    bool find(ptrdiff_t offset)
    {
        target = hook::pattern(aob).get_first(offset);
        return true;
    }
};


namespace patterns
{
    namespace events
    {
        // 8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 46 28 C1 E8 0A
        // offset =
        PATTERN automobile = "8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 46 28 C1 E8 0A";
    }
}


void FindPatterns()
{
    Console::log("ATM : ",std::hex, patterns::events::automobile.find(9));
}

#endif

DWORD WINAPI gta4::MainThread()
{
    Console::Open();
    Console::log("Hello, World!");
    //CMetaData::init();
    //Console::log("Base : ", std::hex, CMetaData::begin());
    #ifndef NO_UI
    d9::hDDLModule = globals::hDll;
    d9::HookDirectX();
    #endif
    #ifndef FEATURE_FIX_IV_SDK
    rage::scr::hookScr();
    #endif
    #ifdef FEATURE_FIX_IV_SDK
    FindPatterns();
    #endif
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
    #ifndef FEATURE_FIX_IV_SDK
    rage::scr::unhookScr();
    #endif
    #ifndef NO_UI
    d9::UnHookDirectX();
    #endif
}