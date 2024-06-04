#ifndef EVENT_HOOKS_HPP
#define EVENT_HOOKS_HPP
#include <class/CTheScripts.hpp>

namespace processScriptsEvent
{
    uint8_t threadDummy[256];
    uintptr_t returnAddress;
    std::list<void(*)()> funcPtrs;

    void Run()
		{
			auto bak = CTheScripts::get()->CurrentThread();
			CTheScripts::get()->CurrentThread() = (uint32_t)threadDummy;

			for (auto& f : funcPtrs)
			{
				f();
			}

			CTheScripts::get()->CurrentThread() = bak;
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
		// CGame::Process
		void Add(void(*funcPtr)())
		{
			funcPtrs.emplace_back(funcPtr);
		}
}

#endif /* EVENT_HOOKS_HPP */
