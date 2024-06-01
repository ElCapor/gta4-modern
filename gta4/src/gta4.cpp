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
//#define NO_UI
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

    void* get(std::uint32_t index, ptrdiff_t offset)
    {
        target = hook::pattern(aob).get(index).get<void>(offset);
        return target;
    }
};

/// @brief Eyestep utilities that make your life easier
/// @author ElCapor
namespace EyeStep
{
namespace util
{

/// @brief Check if the specified address corresponds to a specific instruction
/// @param address target address
/// @param instruction the instruction we expect
/// @return true if iinstruction else false
bool is_instruction(std::uint32_t address, std::uint8_t instruction)
{
    return EyeStep::util::readByte(address) == instruction;
}

/// @brief Compare the value of the address to an array of instructions
/// @param address address to check the instructions at
/// @param instructions the instructions
/// @param align_bytes the number of bytes the address should be aligned to, defaults to one
/// @return true if same else false
bool compare_instructions(std::uint32_t address, std::vector<std::uint8_t> instructions, int align_bytes = 1)
{
    if (address % align_bytes !=0)
        return false;
        
    for (int i =0; i <instructions.size(); i++)
    {
        if (!is_instruction(address + i, instructions[i]))
        {
            return false;
        }
        
    }
    return true;
}

/// @brief Step forward in memory by a number of bytes
/// @param address reference to the address we want to step, defaults to 1
/// @param step the number of steps in byte
void GoForward(std::uint32_t& address, unsigned int step = 1) // unsigned int, if u wanna go back ward use the func dedicated to it
{
    address += abs((int)step);
}

/// @brief Step backward in memory by a number of bytes
/// @param address reference to the address we want to step, defaults to 1
/// @param step the number of steps in byte
void GoBackward(std::uint32_t& address, unsigned int step = 1)
{
    address -= abs((int)step);
}

/// @brief Get the address of the next specified instruction set. NOTE : this is not an aob scan , but can be used to reach some specific instructions after an aob scan
/// @param instructions a byte array containing the target instructions
/// @param max_distance the max distance from the current address
/// @param address the current address we should start searching at
/// @param align_byes number of bytes the address should be aligned to, defaults to 1
/// @return the address the instruction was found at else 0
std::uint32_t get_next_instruction_set(std::uint32_t address,std::vector<uint8_t> instructions, int max_distance = 50, int align_bytes = 1)
{
    int distance = 0; // current distance from the address
    while (!compare_instructions(address, instructions) || distance <= max_distance)
    {
        GoForward(address);
        distance++;
    }
    return address;
}

/// @brief Get the address of the next specified instruction set. NOTE : this is not an aob scan , but can be used to reach some specific instructions after an aob scan
/// @param instructions a byte array containing the target instructions
/// @param max_distance the max distance from the current address
/// @param address the current address we should start searching at
/// @param align_byes number of bytes the address should be aligned to, defaults to 1
/// @return the address the instruction was found at else 0
std::uint32_t get_previous_instruction_set(std::uint32_t address,std::vector<uint8_t> instructions, int max_distance = 50, int align_bytes = 1)
{
    int distance = 0; // current distance from the address
    while (!compare_instructions(address, instructions, align_bytes) || distance <= max_distance)
    {
        GoBackward(address);
        distance++;
    }
    return address;
}

/// @brief Get the prologue of a function who's prologue is not push ebp , but sub esp
/// @param start start address
/// @param next search forward in memory if true, else backward if false
/// @param max_distance max distance from start address
/// @param align_btes number of bytes the address should be aligned to, defaults to 1
/// @return address of the found prologue
std::uint32_t get_sub_esp_prologue(std::uint32_t start, bool next = true, int max_distance = 50, int align_bytes = 1)
{
    return next ? get_next_instruction_set(start, {0x81, 0xEC}, max_distance, align_bytes) : get_previous_instruction_set(start, {0x81, 0xEC}, max_distance, align_bytes);
}

}
}

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
                auto res = xref_result[0];
                auto func = EyeStep::util::get_sub_esp_prologue(res, false, 100);
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

                address = EyeStep::util::get_previous_instruction_set(address, {0x83, 0xEC}, 100);
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

    namespace pools {
        PATTERN ms_PedPool = "64 A1 2C 00 00 00 6A 00 8B 00 6A 10 8B 48 08 6A 1C 8B 01 FF 50 08"; // index 2
        std::uint32_t GetPedPool()
        {
            std::uint32_t ret = 0;
            auto ptr = (std::uint32_t)hook::pattern(ms_PedPool.aob).get(2).get<void>(36);
            ret = EyeStep::util::readInt(ptr+1);
            return ret;
        }
    }

    namespace rage {

        std::uint32_t get_d3d_device()
        {
            auto ptr = EyeStep::scanner::scan_xrefs("GTA IV cannot be launched over remote desktop.")[0];
            //TODO : restore bytes alignement, maybe substract 2 or 3 to ptr
            ptr = EyeStep::util::get_next_instruction_set(ptr, {0x89, 0x0D}, 100);
            Console::log("ptr", ptr);
            return EyeStep::util::readInt(ptr+2);
        }
        namespace rage_scr_thread
        {
            //? The code was cleaned up, it won't blow anyone's pc anymore
            std::uint32_t get_running_thread()
            {
                auto ptr = EyeStep::scanner::scan_xrefs("ERROR!!! Unknown script name ERROR!!!")[0];
                ptr = EyeStep::util::get_previous_instruction_set(ptr, {0x56, 0x8B, 0x71}, 50, 4);
                auto xref_result = EyeStep::scanner::scan_xrefs(ptr);
                auto get_name_of_current_script = xref_result[0]; // jmp getscriptname
                auto relative_call = get_name_of_current_script - 7;
                auto resolve_call = EyeStep::util::getRel(relative_call); // resolve the relative call
                auto m_p_current_thread = EyeStep::util::readInt(resolve_call + 1);
                return m_p_current_thread;
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

uintptr_t DoHook(uintptr_t address, void(*Function)())
{
    if (address) return (uintptr_t)injector::MakeCALL(address, Function);
    return 0;
}

namespace ingameStartupEvent
{
    uint8_t threadDummy[256];
    uintptr_t returnAddress;
    std::list<void(*)()> funcPtrs;

    void Run()
    {
        for (auto& f : funcPtrs)
        {
            f();
        }
    }
    void __declspec(naked) MainHook()
    {
        __asm
        {
            pushad
            call Run
            popad
            jmp returnAddress
        }
    }
    // runs right before loading a save, starting a new game, switching episodes, etc.
    // use this to clean things up
    void Add(void(*funcPtr)())
    {
        funcPtrs.emplace_back(funcPtr);
    }
};
#include "class/CPool.hpp"
#include "class/CPed.hpp"

DWORD WINAPI pool_thread(LPVOID lpParam)
{
    CPool<CPed>* pool = static_cast<CPool<CPed>*>(lpParam);
    while (true)
    {
        Console::log("Pool info, entry size : ",
        pool->m_nEntrySize,
        " count ", 
        pool->m_nCount,
        " top ",
        pool->m_nTop);
        if (pool->m_nEntrySize > 0)
        {
            for (int i = 0; i < pool->m_nCount; i++)
	        {
                if (auto ped = pool->Get(i))
                {
                    Console::log("Hello ped");
                }
            }
        }
        Sleep(10000);
    }
    return 0;
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
    auto m_get_current_thread = findpattern<std::uint32_t>("current thread", patterns::rage::rage_scr_thread::get_running_thread);
    auto device3d = findpattern<std::uint32_t>("D3D Device", patterns::rage::get_d3d_device);

    //DoHook(processHookEvent, ingameStartupEvent::MainHook);

    auto ms_PedPool = findpattern<std::uint32_t>("ms ped pool", patterns::pools::GetPedPool);
    auto pool = reinterpret_cast<CPool<CPed>*>(ms_PedPool);
    Console::log("Pool info, entry size : ",
    pool->m_nEntrySize,
    " count ", 
    pool->m_nCount,
    " top ",
    pool->m_nTop);
    //CreateThread(0, 0, (LPTHREAD_START_ROUTINE)pool_thread, pool, 0, 0);
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
    dinput::InitHook();
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