#ifndef DINPUT_HPP
#define DINPUT_HPP
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
/// @brief Direct Input Hook
/// @authors modified by ElCapor from ,itsclonkandre (iv sdk net), modified from (InGameTimecycEditor) by akifle47.
namespace dinput
{
    namespace hook
    {
        HRESULT __stdcall DInput8DeviceGetDeviceStateHook(IDirectInputDevice8* device, DWORD cbData, LPVOID lpvData);
        HRESULT __stdcall DInput8DeviceGetDeviceDataHook(IDirectInputDevice8* device, DWORD cbData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD flags);
        HRESULT __stdcall DInput8DeviceAcquireHook(IDirectInputDevice8* device);
    }
    void InitHook();
    bool GetVTable(void** table);
}

#endif /* DINPUT_HPP */
