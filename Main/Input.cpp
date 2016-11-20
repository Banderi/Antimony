#include <string>

#include "Warnings.h"
#include "Input.h"
#include "Window.h"
#include "DebugWin.h"

#pragma comment(lib, "Xinput9_1_0.lib")

// TODO: Implement controller input

RAWINPUTDEVICE rid[4];

MouseController mouse;
KeysController keys;
XInputController controller[4];

//

void Axis::Update(float v)
{
	oldpos = pos;
	vel = v;
	pos = oldpos + vel;
}

void Input::Update(char down)
{
	if (down == 1) // input is activated
	{
		if (press != 2)
		{
			press = 1;
			time = clock();
		}
		else
			press = 2;
	}
	else if (down == 0) // input is released
	{
		press = 3;
		time = -1;
	}
	else if (down == -1) // input does not change state
	{
		if (press == 1)
			press = 2;
		else if (press == 3)
		{
			press = 0;
			time = -1;
		}
	}
}
unsigned char Input::GetState()
{
	return press;
}
float Input::GetTime()
{
	return (float)(clock() - time) / CLOCKS_PER_SEC;
}
void Input::Set(std::string nm)
{
	name = nm;
}

void Input_Mouse::Set(unsigned short flup, unsigned short fldn, std::string nm)
{
	flagup = flup;
	flagdown = fldn;
	name = nm;
}
unsigned short Input_Mouse::GetFlagUp()
{
	return flagup;
}
unsigned short Input_Mouse::GetFlagDown()
{
	return flagdown;
}

void Input_Key::Set(unsigned short vk, std::string nm)
{
	vkey = vk;
	name = nm;
}
unsigned short Input_Key::GetVKey()
{
	return vkey;
}

void Input_Button::Set(unsigned short mp, std::string nm)
{
	map = mp;
	name = nm;
}
unsigned short Input_Button::GetMap()
{
	return map;
}

//

void MouseController::Update(RAWMOUSE rmouse)
{
	bool relative = (rmouse.usFlags & MOUSE_MOVE_RELATIVE);

	X.Update((float)rmouse.lLastX); // "Last" is relative position a.k.a velocity
	Y.Update((float)rmouse.lLastY);

	if (rmouse.usButtonFlags & RI_MOUSE_WHEEL)
		Z.Update((short)rmouse.usButtonData);
}
void MouseController::Reset()
{
	X.Update(0);
	Y.Update(0);
	Z.Update(0);
}

void KeysController::UpdateMouse(RAWMOUSE rmouse)
{
	USHORT flags = rmouse.usButtonFlags;

	for (unsigned char i = 0; i < sizeof(mousearray) / sizeof(Input_Mouse*); i++)
	{
		if (flags & mousearray[i]->GetFlagDown())
			mousearray[i]->Update(true);
		else if (flags & mousearray[i]->GetFlagUp())
			mousearray[i]->Update(false);
	}
}
void KeysController::UpdateKeyboard(RAWKEYBOARD rkeys)
{
	USHORT vk = rkeys.VKey;
	
	for (unsigned char i = 0; i < sizeof(keyarray) / sizeof(Input_Key*); i++)
	{
		if (vk == keyarray[i]->GetVKey())
		{
			if (rkeys.Message == WM_KEYDOWN)
				keyarray[i]->Update(true);
			else if (rkeys.Message == WM_KEYUP)
				keyarray[i]->Update(false);
		}
	}
}
void KeysController::Reset()
{
	for (unsigned char i = 0; i < sizeof(mousearray) / sizeof(Input_Mouse*); i++)
	{
		mousearray[i]->Update(-1);
	}
	for (unsigned char i = 0; i < sizeof(keyarray) / sizeof(Input_Key*); i++)
	{
		keyarray[i]->Update(-1);
	}
}
void KeysController::SetKey(Input_Key *key, unsigned short vk, std::string nm)
{
	key->Set(vk, nm);
}
unsigned short KeysController::GetKey(Input_Key *key)
{
	return key->GetVKey();
}

void XInputController::Update()
{
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	enabled = false;

	if (XInputGetState(number, &state) == ERROR_SUCCESS) // controller is connected
		enabled = true;
	else
		return;

	if (enabled)
	{
		float rx = (float)state.Gamepad.sThumbRX / 32768;
		float ry = (float)state.Gamepad.sThumbRY / 32768;
		float lx = (float)state.Gamepad.sThumbLX / 32768;
		float ly = (float)state.Gamepad.sThumbLY / 32768;

		float mag_r = sqrt(rx * rx + ry * ry);
		float mag_l = sqrt(lx * lx + ly * ly);
		float dead_r = 0.25f;
		float dead_l = 0.25f;

		mag_r = min(mag_r, 1);
		mag_l = min(mag_l, 1);

		if (mag_r > dead_r)
		{
			float mult = (mag_r - dead_r) / (1.0f - dead_r);
			RX.Update(rx * mult);
			RY.Update(ry * mult);
		}
		else
		{
			RX.Update(0);
			RY.Update(0);
		}
		if (mag_l > dead_l)
		{
			float mult = (mag_l - dead_l) / (1.0f - dead_l);
			LX.Update(lx * mult);
			LY.Update(ly * mult);
		}
		else
		{
			LX.Update(0);
			LY.Update(0);
		}

		RZ = state.Gamepad.bRightTrigger;
		LZ = state.Gamepad.bLeftTrigger;
		if (RZ > 0)
			RT.Update(true);
		else if (RT.GetState() != unpressed)
			RT.Update(false);
		if (LZ > 0)
			LT.Update(true);
		else if (LT.GetState() != unpressed)
			LT.Update(false);

		for (unsigned char i = 0; i < sizeof(btnarray) / sizeof(Input_Key*); i++)
		{
			if (btnarray[i]->GetMap() != 0)
			{
				if (state.Gamepad.wButtons & btnarray[i]->GetMap())
					btnarray[i]->Update(true);
				else if (btnarray[i]->GetState() != unpressed)
					btnarray[i]->Update(false);
			}			
		}
	}
}
void XInputController::Reset()
{
	for (unsigned char i = 0; i < sizeof(btnarray) / sizeof(Input_Key*); i++)
	{
		if (btnarray[i]->GetMap() != 0)
			btnarray[i]->Update(-1);
		else
		{
			btnarray[i]->Update(-1);
		}
	}
}
void XInputController::Vibrate(float leftmotor, float rightmotor)
{
	// Create a new Vibraton 
	XINPUT_VIBRATION Vibration;

	memset(&Vibration, 0, sizeof(XINPUT_VIBRATION));

	int leftVib = (int)(leftmotor*65535.0f);
	int rightVib = (int)(rightmotor*65535.0f);

	// Set the Vibration Values
	Vibration.wLeftMotorSpeed = leftVib;
	Vibration.wRightMotorSpeed = rightVib;
	// Vibrate the controller
	XInputSetState(number, &Vibration);
}

//

HRESULT RegisterRID()
{
	WriteToConsole(L"Initializing RID objects... ");

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;				// mouse
	rid[0].dwFlags = RIDEV_NOLEGACY;
	rid[0].hwndTarget = windowMain.hWnd;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;				// keyboard
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = windowMain.hWnd;

	rid[2].usUsagePage = 0x01;
	rid[2].usUsage = 0x05;				// gamepad
	rid[2].dwFlags = 0;
	rid[2].hwndTarget = windowMain.hWnd;

	rid[3].usUsagePage = 0x01;
	rid[3].usUsage = 0x04;				// joystick
	rid[3].dwFlags = 0;
	rid[3].hwndTarget = windowMain.hWnd;

	if (RegisterRawInputDevices(rid, 4, sizeof(rid[0])) == false)
	{
		LogError(HRESULT_FROM_WIN32(GetLastError()));
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		XINPUT_STATE state;
		for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
		{ 
			controller[i].enabled = false;
			controller[i].number = i;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			if (XInputGetState(i, &state) == ERROR_SUCCESS) // controller is connected
				controller[i].enabled = true;
		}

		WriteToConsole(L"done\n");
		return S_OK;
	}
}
HRESULT HandleRaw(MSG msg)
{
	//HRESULT hr;
	UINT dwSize;

	GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
	LPBYTE lpb = new BYTE[dwSize];
	if (lpb == NULL)
		return 0;

	int readSize = GetRawInputData((HRAWINPUT)msg.lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
	if (readSize != dwSize)
		return HRESULT_FROM_WIN32(GetLastError());

	RAWINPUT* raw = (RAWINPUT*)lpb;

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		keys.UpdateKeyboard(raw->data.keyboard);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		keys.UpdateMouse(raw->data.mouse);
		mouse.Update(raw->data.mouse);
	}
	else if (raw->header.dwType == RIM_TYPEHID)
	{
		for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
		{
			controller[i].Update();
		}
	}

	delete[] lpb;

	return S_OK;
}