#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
#include "feature/rage/scr/scrThreadHook.hpp"
#include <eyestep/eyestep.h>
#include <eyestep/eyestep_utility.h>
#include <functional>
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
        PATTERN inGameStartup = "56 E8 ? ? ? ? A3 ? ? ? ? 89 15 ? ? ? ? E8 ? ? ? ?";
        PATTERN gameEvent = "83 ? ? 3B ? 7D ? 8B 1F 8B B6 80 00 00 00 8B ? 69 ? 88 00 00 00"; // this requires a check for D:E
        PATTERN FuncAboveProcessHook = "55 8B EC 83 E4 F0 81 EC 84 00 00 00 83 3D ? ? ? ? 00"; // aob to the func on top of process hook
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

            std::uint32_t GetMountDeviceCall()
            {
                auto xref_result = EyeStep::scanner::scan_xrefs("commonimg:/");//
                auto func = xref_result[0];
                bool done = false;
                while (!done) // a bit hacky but i got no choice ong
                {
                    func--;
                    if (func%16==0)
                    {
                        if (EyeStep::util::readByte(func) == 0x81 && EyeStep::util::readByte(func+1) == 0xEC)
                            done = true;
                    }
                }
                xref_result = EyeStep::scanner::scan_xrefs(func);
                func = xref_result[0];
                return func;
            }

            std::uint32_t GetLoadEventCall()
            {
                auto pattern = hook::pattern(gameEvent.aob);
                std::uint32_t address = 0;
                if (pattern.size() > 1)
                {
                    address = (std::uint32_t)pattern.get(2).get<std::uint32_t>(0);
                }
                else {
                    address = (std::uint32_t)pattern.get_first(0);
                }

                bool done = false;
                while (!done) // a bit hacky but i got no choice ong
                {
                    address--;
                    if (address%16==0)
                    {
                        if ((EyeStep::util::readByte(address) == 0x81 || EyeStep::util::readByte(address) == 0x83) && EyeStep::util::readByte(address+1) == 0xEC)
                            done = true;
                    }
                }
                auto xref_result = EyeStep::scanner::scan_xrefs(address);
                address = xref_result[0];
                return address;
            }
            
            std::uint32_t GetProcessHookAddres()
            {
                auto abovefunc = FuncAboveProcessHook.find(0);
                auto xref_results = EyeStep::scanner::scan_xrefs((std::uint32_t)abovefunc);
                auto callAbove = xref_results[0];
                return callAbove + 5;
            }
        }
    }
}


void SetupEyestep()
{
    EyeStep::open(GetCurrentProcess());
}

template <typename T>
T findpattern(const char* name, std::function<std::uint32_t()> fn)
{
    Console::log("[PATTERN] Looking up ", name, "...");
    std::uint32_t result = fn();
    Console::log("[PATTERN] RESULT : ", std::hex, result);
    return static_cast<T>(result);

}

template <typename T>
T findpattern(const char* name, std::function<void*()> fn)
{
    return findpattern<T>(name, [fn]() -> std::uint32_t {return reinterpret_cast<std::uint32_t>(fn());});
}
void FindPatterns()
{
    SetupEyestep();
    auto atm = findpattern<std::uint32_t>("processAutomobileEvent", []() -> void* {return patterns::events::automobile.find(9);});
    auto pad = findpattern<std::uint32_t>("processAutomobileEvent", []() -> void* {return patterns::events::pad.find(2);});
    auto camera = findpattern<std::uint32_t>("camera event", []() -> void* {return patterns::events::camera.find(2);});
    auto drawing = findpattern<std::uint32_t>("drawing event", []() -> void* {return patterns::events::drawing.find(8);});
    auto loadEventPriority = findpattern<std::uint32_t>("event priority", patterns::events::hard::GetPopulationConfigCall);
    auto inGameStartup = findpattern<std::uint32_t>("in game startup", []() -> void* {return patterns::events::inGameStartup.find(17);});
    auto mountDeviceEvent = findpattern<std::uint32_t>("mount device event", patterns::events::hard::GetMountDeviceCall);
    auto loadEvent = findpattern<std::uint32_t>("event priority", patterns::events::hard::GetLoadEventCall);
    auto processHookEvent = findpattern<std::uint32_t>("process hook", patterns::events::hard::GetProcessHookAddres);

    /*
    Console::log("PAD : ",std::hex, patterns::events::pad.find(2));
    Console::log("CAMERA : ",std::hex, patterns::events::camera.find(2));
    Console::log("DRAWING : ", std::hex, patterns::events::drawing.find(8));
    Console::log("EVENT PRIORITY ", std::hex, patterns::events::hard::GetPopulationConfigCall());
    Console::log("IN GAME STARTUP", std::hex, patterns::events::inGameStartup.find(17));
    Console::log("Mount device event" , std::hex, patterns::events::hard::GetMountDeviceCall());
    Console::log("Get Load Event ...", std::hex, patterns::events::hard::GetLoadEventCall());
    Console::log("ProcessHook..", std::hex, patterns::events::hard::GetProcessHookAddres());
    */
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