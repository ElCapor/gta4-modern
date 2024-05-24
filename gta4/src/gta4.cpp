#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
#include "feature/rage/scr/scrThreadHook.hpp"
#include <eyestep/eyestep.h>
#include <eyestep/eyestep_utility.h>
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

    void* find(ptrdiff_t offset)
    {
        target = hook::pattern(aob).get_first(offset);
        return target;
    }
};


namespace patterns
{
    namespace events
    {
        PATTERN automobile = "8B CE E8 ? ? ? ? 8B CE E8 ? ? ? ? 8B 46 28 C1 E8 0A";
        PATTERN pad = "8B CE E8 ? ? ? ? 81 C6 84 3A 00 00";
        PATTERN camera = "8B CE E8 ? ? ? ? 5F 5E B0 01 5B C3";
        PATTERN drawing = "E8 ? ? ? ? 83 C4 08 E8 ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? 00 74 ?";
        namespace hard { // these are hard to get
            std::uint32_t GetPopulationConfigCall() // aka load event prior to the game launching
            {
                auto xref_result = EyeStep::scanner::scan_xrefs("common:/data/pedVariations.dat");
                auto func = xref_result[0];
                // we can't get prologue since it's a near function
                func -= 7;
                auto new_xref = EyeStep::scanner::scan_xrefs(func);
                return new_xref[0];
            }
        }
    }
}


void SetupEyestep()
{
    EyeStep::open(GetCurrentProcess());
}

void FindPatterns()
{
    SetupEyestep();
    Console::log("ATM : ",std::hex, patterns::events::automobile.find(9));
    Console::log("PAD : ",std::hex, patterns::events::pad.find(2));
    Console::log("CAMERA : ",std::hex, patterns::events::camera.find(2));
    Console::log("DRAWING : ", std::hex, patterns::events::drawing.find(8));
    Console::log("POPULATION STR ", std::hex, patterns::events::populationConfig.find(0));
    
    Console::log("XREF RESULT : ", std::hex, new_xref[0]);
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