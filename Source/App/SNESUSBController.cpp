// SNES USB controller
// https://github.com/r57zone

#include <windows.h>
#include <mutex>
#include <iostream>
#include "ViGEm\Client.h"
#include "IniReader\IniReader.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

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

static std::mutex m;
VOID CALLBACK notification(
	PVIGEM_CLIENT Client,
	PVIGEM_TARGET Target,
	UCHAR LargeMotor,
	UCHAR SmallMotor,
	UCHAR LedNumber,
	LPVOID UserData
)
{
	m.lock();
	m.unlock();
}

void MainText(bool GamepadStatus, bool BumpersIsTriggers, bool DPADIsStick) {
	system("cls");
	printf("\n");
	if (GamepadStatus)
		printf(" SNES USB controller connected\n");
	else
		printf(" SNES USB controller connected\n");
	printf(" Bumpers is triggers = ");
	if (BumpersIsTriggers)
		printf("yes | ");
	else
		printf("no | ");
	printf("DPAD is stick = ");
	if (DPADIsStick)
		printf("yes\n");
	else
		printf("no\n");

	printf(" Press Select + DPAD up or ALT + Q to swap bumpers and triggers.\n");
	printf(" Press Select + DPAD left or ALT + A to swap left stick and DPAD.\n");

}

int main(int argc, char **argv)
{
	SetConsoleTitle("SNES USB controller 1.0");
	// Config parameters
	CIniReader IniFile("Config.ini");

	int SleepTimeOut = IniFile.ReadInteger("Gamepad", "SleepTimeOut", 1);
	
	bool BumpersIsTriggers = IniFile.ReadBoolean("Gamepad", "BumpersIsTriggers", true);
	bool DPADIsStick = IniFile.ReadBoolean("Gamepad", "DPADIsStick", true);

	int SkipPollCount = 0;

	const auto client = vigem_alloc();
	auto ret = vigem_connect(client);

	const auto x360 = vigem_target_x360_alloc();
	ret = vigem_target_add(client, x360);
	ret = vigem_target_x360_register_notification(client, x360, &notification, nullptr);

	XUSB_REPORT report;

	JOYINFOEX joyInfo;
	joyInfo.dwFlags = JOY_RETURNALL;
	joyInfo.dwSize = sizeof(joyInfo);

	int JoyIndex = JOYSTICKID1;

	if (joyGetPosEx(JOYSTICKID1, &joyInfo) == JOYERR_NOERROR)
		JoyIndex = JOYSTICKID1;
	else if (joyGetPosEx(JOYSTICKID2, &joyInfo) == JOYERR_NOERROR)
		JoyIndex = JOYSTICKID2;

	MainText(joyGetPosEx(JoyIndex, &joyInfo) == JOYERR_NOERROR, BumpersIsTriggers, DPADIsStick);

	while (! ( GetAsyncKeyState(VK_LMENU) & 0x8000 && GetAsyncKeyState(VK_ESCAPE) & 0x8000 ) )
	{
		XUSB_REPORT_INIT(&report);

		joyGetPosEx(JoyIndex, &joyInfo);

		// Dead zones
		if (((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && (GetAsyncKeyState('Q') & 0x8000) != 0 && SkipPollCount == 0) {
			BumpersIsTriggers = !BumpersIsTriggers;
			MainText(joyGetPosEx(JoyIndex, &joyInfo) == JOYERR_NOERROR, BumpersIsTriggers, DPADIsStick);
			SkipPollCount = 15;
		}
		if (((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) && (GetAsyncKeyState('A') & 0x8000) != 0 && SkipPollCount == 0) {
			DPADIsStick = !DPADIsStick;
			MainText(joyGetPosEx(JoyIndex, &joyInfo) == JOYERR_NOERROR, BumpersIsTriggers, DPADIsStick);
			SkipPollCount = 15;
		}

		if (DPADIsStick) {
			if (joyInfo.dwXpos > 32511) report.sThumbLX = 32767;
			else if (joyInfo.dwXpos < 32511) report.sThumbLX = -32767;
			else report.sThumbLX = 0;

			if (joyInfo.dwYpos > 32511) report.sThumbLY = -32767;
			else if (joyInfo.dwYpos < 32511) report.sThumbLY = 32767;
			else report.sThumbLY = 0;
		} else {
			if (joyInfo.dwXpos > 32511) report.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
			else if (joyInfo.dwXpos < 32511) report.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
			else report.sThumbLX = 0;

			if (joyInfo.dwYpos > 32511) report.sThumbLY = report.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
			else if (joyInfo.dwYpos < 32511) report.sThumbLY = report.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
			else report.sThumbLY = 0;
		}

		if (joyInfo.dwButtons & JOY_BUTTON9 && SkipPollCount == 0) {
			if (joyInfo.dwXpos < 32511 || joyInfo.dwYpos < 32511) {
				if (joyInfo.dwXpos < 32511) {
					DPADIsStick = !DPADIsStick;
					SkipPollCount = 15;
				} else if (joyInfo.dwYpos < 32511) {
					BumpersIsTriggers = !BumpersIsTriggers;
					SkipPollCount = 15;
				}
				MainText(joyGetPosEx(JoyIndex, &joyInfo) == JOYERR_NOERROR, BumpersIsTriggers, DPADIsStick);
			} else
				report.wButtons |= XINPUT_GAMEPAD_BACK;
		}
		
		report.wButtons |= joyInfo.dwButtons & JOY_BUTTON10 ? XINPUT_GAMEPAD_START : 0;

		if (BumpersIsTriggers) {
			report.bLeftTrigger = joyInfo.dwButtons & JOY_BUTTON5 ? 255 : 0;
			report.bRightTrigger = joyInfo.dwButtons & JOY_BUTTON6 ? 255 : 0;
		} else {
			report.wButtons |= joyInfo.dwButtons & JOY_BUTTON5 ? XINPUT_GAMEPAD_LEFT_SHOULDER : 0;
			report.wButtons |= joyInfo.dwButtons & JOY_BUTTON6 ? XINPUT_GAMEPAD_RIGHT_SHOULDER : 0;
		}
			
		report.wButtons |= joyInfo.dwButtons & JOY_BUTTON4 ? XINPUT_GAMEPAD_X : 0;
		report.wButtons |= joyInfo.dwButtons & JOY_BUTTON3 ? XINPUT_GAMEPAD_A : 0;
		report.wButtons |= joyInfo.dwButtons & JOY_BUTTON2 ? XINPUT_GAMEPAD_B : 0;
		report.wButtons |= joyInfo.dwButtons & JOY_BUTTON1 ? XINPUT_GAMEPAD_Y : 0;

		if (SkipPollCount > 0) SkipPollCount--;
		
		ret = vigem_target_x360_update(client, x360, report);

		Sleep(SleepTimeOut);
	}

	vigem_target_x360_unregister_notification(x360);
	vigem_target_remove(client, x360);
	vigem_target_free(x360);
}
