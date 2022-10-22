#include <windows.h>
#include "MinHook.h"
#include <iostream>
#include <algorithm>
#include <mmsystem.h>
#include <atlstr.h> 

#if defined _M_X64
#pragma comment(lib, "libMinHook-x64-v141-md.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook-x86-v141-md.lib")
#endif

#pragma comment(lib, "winmm.lib")

#define ERROR_DEVICE_NOT_CONNECTED		1167
#define XINPUT_GAMEPAD_DPAD_UP          0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN        0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT        0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT       0x0008
#define XINPUT_GAMEPAD_GUIDE            0x0400
#define XINPUT_GAMEPAD_START            0x0010
#define XINPUT_GAMEPAD_BACK             0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB       0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB      0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
#define XINPUT_GAMEPAD_A                0x1000
#define XINPUT_GAMEPAD_B                0x2000
#define XINPUT_GAMEPAD_X                0x4000
#define XINPUT_GAMEPAD_Y				0x8000


typedef struct _XINPUT_GAMEPAD
{
	WORD                                wButtons;
	BYTE                                bLeftTrigger;
	BYTE                                bRightTrigger;
	SHORT                               sThumbLX;
	SHORT                               sThumbLY;
	SHORT                               sThumbRX;
	SHORT                               sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE
{
	DWORD                               dwPacketNumber;
	XINPUT_GAMEPAD                      Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;


typedef DWORD(WINAPI *XINPUTGETSTATE)(DWORD, XINPUT_STATE*);

// Pointer for calling original
static XINPUTGETSTATE hookedXInputGetState = nullptr;

// wrapper for easier setting up hooks for MinHook
template <typename T>
inline MH_STATUS MH_CreateHookEx(LPVOID pTarget, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHook(pTarget, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

template <typename T>
inline MH_STATUS MH_CreateHookApiEx(LPCWSTR pszModule, LPCSTR pszProcName, LPVOID pDetour, T** ppOriginal)
{
	return MH_CreateHookApi(pszModule, pszProcName, pDetour, reinterpret_cast<LPVOID*>(ppOriginal));
}

bool BumpersIsTriggers = true;
bool DPADIsStick = true;
int SkipPollCount = 0;
int JoyIndex = JOYSTICKID1;
JOYINFOEX joyInfo;

bool Init = false;
int WorkStatus = 0;

//Own GetState
DWORD WINAPI detourXInputGetState(DWORD dwUserIndex, XINPUT_STATE* pState)
{
	if (Init == false) {
		joyInfo.dwFlags = JOY_RETURNALL;
		joyInfo.dwSize = sizeof(joyInfo);
		Init = true;

		TCHAR HardwareInsertWav[MAX_PATH] = { 0 };
		TCHAR HardwareFailWav[MAX_PATH] = { 0 };
		GetSystemWindowsDirectory(HardwareInsertWav, sizeof(HardwareInsertWav));
		_tcscpy_s(HardwareFailWav, sizeof(HardwareInsertWav), HardwareInsertWav);
		_tcscat_s(HardwareInsertWav, sizeof(HardwareInsertWav), _T("\\Media\\Windows Hardware Insert.wav"));
		_tcscat_s(HardwareFailWav, sizeof(HardwareFailWav), _T("\\Media\\Windows Hardware Fail.wav"));

		if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR || joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR && WorkStatus > 0)
			PlaySound(HardwareInsertWav, NULL, SND_ASYNC); //Alarm04.wav
		else
			PlaySound(HardwareFailWav, NULL, SND_ASYNC); 
	}

	//ZeroMemory(pState, sizeof(XINPUT_STATE));

	// first call the original function
	DWORD toReturn = hookedXInputGetState(dwUserIndex, pState);

	pState->Gamepad.sThumbLX = 0;
	pState->Gamepad.sThumbLY = 0;
	pState->Gamepad.sThumbRX = 0;
	pState->Gamepad.sThumbRY = 0;
	pState->Gamepad.wButtons = 0;
	pState->Gamepad.bLeftTrigger = 0;
	pState->Gamepad.bRightTrigger = 0;

	if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR) {
		JoyIndex = JOYSTICKID1;
		toReturn = ERROR_SUCCESS;
	} else if (joyGetPosEx(JOYSTICKID2, &joyInfo) == JOYERR_NOERROR) {
		 JoyIndex = JOYSTICKID2;
		 toReturn = ERROR_SUCCESS;
	} else
		toReturn = ERROR_DEVICE_NOT_CONNECTED;

	if (toReturn == ERROR_SUCCESS) {

		if (((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && (GetAsyncKeyState('Q') & 0x8000) != 0 && SkipPollCount == 0) {
			BumpersIsTriggers = !BumpersIsTriggers;
			SkipPollCount = 15;
		}
		if (((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && (GetAsyncKeyState('A') & 0x8000) != 0 && SkipPollCount == 0) {
			DPADIsStick = !DPADIsStick;
			SkipPollCount = 15;
		}

		if (DPADIsStick) {
			if (joyInfo.dwXpos > 32511) pState->Gamepad.sThumbLX = 32767;
			else if (joyInfo.dwXpos < 32511) pState->Gamepad.sThumbLX = -32767;
			else pState->Gamepad.sThumbLX = 0;

			if (joyInfo.dwYpos > 32511)pState->Gamepad.sThumbLY = -32767;
			else if (joyInfo.dwYpos < 32511) pState->Gamepad.sThumbLY = 32767;
			else pState->Gamepad.sThumbLY = 0;
		}
		else {
			if (joyInfo.dwXpos > 32511) pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
			else if (joyInfo.dwXpos < 32511) pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
			else pState->Gamepad.sThumbLX = 0;

			if (joyInfo.dwYpos > 32511) pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
			else if (joyInfo.dwYpos < 32511) pState->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
			else pState->Gamepad.sThumbLY = 0;
		}

		if (joyInfo.dwButtons & JOY_BUTTON9 && SkipPollCount == 0) {
			if (joyInfo.dwXpos < 32511 || joyInfo.dwYpos < 32511) {
				if (joyInfo.dwXpos < 32511) {
					DPADIsStick = !DPADIsStick;
					SkipPollCount = 15;
				}
				else if (joyInfo.dwYpos < 32511) {
					BumpersIsTriggers = !BumpersIsTriggers;
					SkipPollCount = 15;
				}
			}
			else
				pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON9 ? XINPUT_GAMEPAD_BACK : 0;
		}

		pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON10 ? XINPUT_GAMEPAD_START : 0;

		if (BumpersIsTriggers) {
			pState->Gamepad.bLeftTrigger = joyInfo.dwButtons & JOY_BUTTON5 ? 255 : 0;
			pState->Gamepad.bRightTrigger = joyInfo.dwButtons & JOY_BUTTON6 ? 255 : 0;
		}
		else {
			pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON5 ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
			pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON6 ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;
		}

		pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON4 ? XINPUT_GAMEPAD_X : 0;
		pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON3 ? XINPUT_GAMEPAD_A : 0;
		pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON2 ? XINPUT_GAMEPAD_B : 0;
		pState->Gamepad.wButtons |= joyInfo.dwButtons & JOY_BUTTON1 ? XINPUT_GAMEPAD_Y : 0;

	}

	if (SkipPollCount > 0) SkipPollCount--;

	return toReturn;
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call){
		case DLL_PROCESS_ATTACH:
		{
			// Injection trick taken here https://github.com/FransBouma/InjectableGenericCameraSystem
			//MessageBox(0, "ATTACH", "XINPUT", MB_OK);
			if (MH_Initialize() == MH_OK) {

				//1.0
				if (MH_CreateHookApiEx(L"XINPUT9_1_0", "XInputGetStateEx", &detourXInputGetState, &hookedXInputGetState) == MH_OK) //Ex! - Bulletstorm (2011)
					WorkStatus++;

				//1_1
				if (hookedXInputGetState == nullptr)
					if (MH_CreateHookApiEx(L"XINPUT_1_1", "XInputGetState", &detourXInputGetState, &hookedXInputGetState) == MH_OK) //Ex?
						WorkStatus++;

				//1_2
				if (hookedXInputGetState == nullptr)
					if (MH_CreateHookApiEx(L"XINPUT_1_2", "XInputGetState", &detourXInputGetState, &hookedXInputGetState) == MH_OK) //Ex?
						WorkStatus++;

				//1_3
				if (hookedXInputGetState == nullptr)
					if (MH_CreateHookApiEx(L"XINPUT1_3", "XInputGetState", &detourXInputGetState, &hookedXInputGetState) == MH_OK)
						WorkStatus++;

				//1_4
				if (hookedXInputGetState == nullptr)
					if (MH_CreateHookApiEx(L"XINPUT1_4", "XInputGetState", &detourXInputGetState, &hookedXInputGetState) == MH_OK)
						WorkStatus++;

				//if (MH_EnableHook(&detourXInputGetState) == MH_OK) //Not working
				if (MH_EnableHook(MH_ALL_HOOKS) == MH_OK)
					WorkStatus++;
			}

			break;
		}

		/*case DLL_THREAD_ATTACH:
		{
			break;
		}

		case DLL_THREAD_DETACH:
		{
			break;
		}*/

		case DLL_PROCESS_DETACH:
		{
			MH_DisableHook(&detourXInputGetState);
			MH_Uninitialize();
			break;
		}
	}
	return true;
}
