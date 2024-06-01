#include <feature/dinput.hpp>
#include <feature/d9hook.hpp>
#include <console.hpp>
#include <detours.h>
typedef HRESULT(__stdcall DInput8DeviceGetDeviceStateT)(IDirectInputDevice8*, DWORD, LPVOID);
typedef HRESULT(__stdcall DInput8DeviceGetDeviceDataT)(IDirectInputDevice8*, DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD);
typedef HRESULT(__stdcall DInput8DeviceAcquireT)(IDirectInputDevice8*);

static DInput8DeviceGetDeviceStateT* g_sDInput8DeviceGetDeviceStateOriginal = nullptr;
static DInput8DeviceGetDeviceDataT* g_sDInput8DeviceGetDeviceDataOriginal = nullptr;
static DInput8DeviceAcquireT* g_sDInput8DeviceAcquireOriginal = nullptr;

HRESULT __stdcall dinput::hook::DInput8DeviceGetDeviceStateHook(IDirectInputDevice8 *device, DWORD cbData, LPVOID lpvData)
{
    HRESULT hr = g_sDInput8DeviceGetDeviceStateOriginal(device, cbData, lpvData);

	if (d9::WantsMouse())
	{
		if (cbData == sizeof(DIMOUSESTATE) || cbData == sizeof(DIMOUSESTATE2))
			device->Unacquire();
	}
    else {
        if (cbData == sizeof(DIMOUSESTATE) || cbData == sizeof(DIMOUSESTATE2))
			device->Acquire();
    }

	return hr;
}

HRESULT __stdcall dinput::hook::DInput8DeviceGetDeviceDataHook(IDirectInputDevice8 *device, DWORD cbData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD flags)
{
    HRESULT hr = g_sDInput8DeviceGetDeviceDataOriginal(device, cbData, rgdod, pdwInOut, flags);

	if (d9::WantsMouse())
		device->Unacquire();
    else
        device->Acquire();

	return hr;
}

HRESULT __stdcall dinput::hook::DInput8DeviceAcquireHook(IDirectInputDevice8 *device)
{
    if (d9::WantsMouse())
		return DI_OK;

	return g_sDInput8DeviceAcquireOriginal(device);
}

void dinput::InitHook()
{
    void* vtable[32];
    if (GetVTable(vtable))
    {
        if (!g_sDInput8DeviceAcquireOriginal && !g_sDInput8DeviceGetDeviceDataOriginal && !g_sDInput8DeviceGetDeviceStateOriginal)
        {
            g_sDInput8DeviceGetDeviceStateOriginal = (DInput8DeviceGetDeviceStateT*)vtable[9];
            g_sDInput8DeviceGetDeviceDataOriginal = (DInput8DeviceGetDeviceDataT*)vtable[10];
            g_sDInput8DeviceAcquireOriginal = (DInput8DeviceAcquireT*)vtable[7];
            DetourTransactionBegin();
	        DetourUpdateThread(GetCurrentThread());
            DetourAttach(&(PVOID&)g_sDInput8DeviceGetDeviceStateOriginal,hook::DInput8DeviceGetDeviceStateHook);
            DetourAttach(&(PVOID&)g_sDInput8DeviceGetDeviceDataOriginal,hook::DInput8DeviceGetDeviceDataHook);
            DetourAttach(&(PVOID&)g_sDInput8DeviceAcquireOriginal,hook::DInput8DeviceAcquireHook);
            DetourTransactionCommit();
        }
    }
    else {
        Console::error("Failed to get d8input device table");
    }
}

bool dinput::GetVTable(void **vtable)
{
    IDirectInput8W* dinput8 = nullptr;

    // Create DirectInput8 device
    if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8W, (void**)&dinput8, 0) != DI_OK)
        return false;

    IDirectInputDevice8W* dummyDevice = nullptr;

    // Create device
    if (dinput8->CreateDevice(GUID_SysKeyboard, &dummyDevice, NULL) != DI_OK)
    {
        dinput8->Release();
        return false;
    }

    // Get vtable
    memcpy(vtable, *(void***)dummyDevice, 32 * 4);

    // Free stuff
    dinput8->Release();
    dummyDevice->Release();

    return true;
}
