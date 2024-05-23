#ifndef SCRTHREADHOOK_HPP
#define SCRTHREADHOOK_HPP
#include <cstdint>
#include <Windows.h>
namespace rage
{

    namespace scr
    {
        class scrThread
        {
        public:
            void *pVTable; //0x0000
            int32_t iThreadId; //0x0004
            void *pHashTable; //0x0008
            int32_t iThreadState; //0x000C
            int32_t iIP; //0x0010
            int32_t iFrameSP; //0x0014
            int32_t iSP; //0x0018
            int32_t iTimerA; //0x001C
            int32_t iTimerB; //0x0020
            int32_t iTimerC; //0x0024
            float fWaitTime; //0x0028
            char pad_002C[44]; //0x002C
            void *pScriptStack; //0x0058
            char pad_005C[16]; //0x005C
            char *tExitMsg; //0x006C
            char name[4]; //0x0070
        }; //Size: 0x070

        void hookScr();
        void unhookScr();
    }

}

#endif /* SCRTHREADHOOK_HPP */
