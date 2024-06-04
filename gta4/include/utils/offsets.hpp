#ifndef OFFSETS_HPP
#define OFFSETS_HPP
#include <utils/mem.hpp>

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

#endif /* OFFSETS_HPP */
