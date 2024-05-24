#ifndef SCRTHREADHOOK_HPP
#define SCRTHREADHOOK_HPP
#include <cstdint>
#include <Windows.h>
namespace rage
{

    namespace scr
    {
        class GtaThread
        {
        public:
        struct {
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
        } scrThread;
        char szProgramName[24]; //0x0070
        int32_t iSavedDeathArrestIP; //0x0088
        int32_t iSavedDeathArrestFrameSP; //0x008C
        int32_t iSavedDeathArrestStackOff; //0x0090
        bool bOnMission; //0x0094
        bool bScriptSafeForNetworkGame; //0x0095
        bool bThisScriptShouldBeSaved; //0x0096
        bool bPlayerControlInMissionCleanup; //0x0097
        bool bClearHelpInMissionCleanup; //0x0098
        bool bMiniGameScript; //0x0099
        bool bAllowNonMiniGameTextMessages; //0x009A
        bool bAllowOneTimeOnlyCommandToRun; //0x009B
        bool bPaused; //0x009C
        bool bCanBePaused; //0x009D
        bool eHandleType; //0x009E
        bool pad; //0x009F
        int32_t iHandle; //0x00A0
        bool bCanRemoveBlips; //0x00A4
        }; //Size: 0x070

        void hookScr();
        void unhookScr();
    }

}

#endif /* SCRTHREADHOOK_HPP */
