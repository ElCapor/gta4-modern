#include "gta4.hpp"
#include "console.hpp"
#include "globals.hpp"
#include "utils/pattern.hpp"
#include "feature/d9hook.hpp"
#include "feature/rage/scr/scrThreadHook.hpp"
#include <eyestep/eyestep.h>
#include <eyestep/eyestep_utility.h>
#include <functional>
#include <injector/injector.hpp>
#include <feature/dinput.hpp>
#include <feature/rage/rage.hpp>
#include <utils/eyestep_extension.hpp>
#include <utils/offsets.hpp>
//#define NO_UI
#define FEATURE_FIX_IV_SDK

#ifdef FEATURE_FIX_IV_SDK


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

uintptr_t DoHook(uintptr_t address, void(*Function)())
{
    if (address) return (uintptr_t)injector::MakeCALL(address, Function);
    return 0;
}



#include "class/CPool.hpp"
#include "class/CPed.hpp"
#include "class/CTheScripts.hpp"
#include "feature/event_hooks.hpp"
#include <feature/d9draw.hpp>
CPool<CPed>* g_pedPool;
class DebugMenu : public d9widget
{
    public:
    void Init() override
    {
        Console::log("Starting Debug Menu lmao");
    }

    void Render(float dt) override
    {
        
    }

};

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
    auto m_get_current_thread = findpattern<std::uint32_t>("current thread", patterns::rage::rage_scr_thread::get_running_thread);
    auto device3d = findpattern<std::uint32_t>("D3D Device", patterns::rage::get_d3d_device);

    CTheScripts::get()->CurrentThread() = m_get_current_thread;
}

#endif

DWORD WINAPI gta4::MainThread()
{
    Console::Open();
    Console::log("Hello, World!");
    //CMetaData::init();
    //Console::log("Base : ", std::hex, CMetaData::begin());
    #ifndef NO_UI
    dinput::InitHook();
    d9::hDDLModule = globals::hDll;
    d9::HookDirectX();
    d9draw::RegisterWidget(new DebugMenu());
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