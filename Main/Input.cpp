#include <string>

#include "Warnings.h"
#include "Input.h"
#include "Window.h"
#include "DebugWin.h"

#pragma comment(lib, "Xinput9_1_0.lib")

///

RAWINPUTDEVICE rid[4];

MouseController mouse;
KeysController keys;
XInputController controller[4];

///

void Axis::catchState(float v)
{
	m_pos += v;
}
void Axis::update()
{
	m_vel = m_pos - m_oldPos;
	m_oldPos = m_pos;
}
float Axis::getPos()
{
	return m_pos;
}
float Axis::getVel()
{
	return m_vel;
}

void Input::update(char down)
{
	if (down == 1) // input is activated
	{
		if (m_pressState != 2)
		{
			m_pressState = 1;
			m_pressTime = clock();
		}
		else
			m_pressState = 2;
	}
	else if (down == 0) // input is released
	{
		m_pressState = 3;
		m_pressTime = -1;
	}
	else if (down == -1) // input does not change state
	{
		if (m_pressState == 1)
			m_pressState = 2;
		else if (m_pressState == 3)
		{
			m_pressState = 0;
			m_pressTime = -1;
		}
	}
}
unsigned char Input::getState()
{
	return m_pressState;
}
float Input::getTime()
{
	return (float)(clock() - m_pressTime) / CLOCKS_PER_SEC;
}
void Input::set(std::string nm)
{
	name = nm;
}

void Input_Mouse::set(unsigned short flup, unsigned short fldn, std::string nm)
{
	m_flagUp = flup;
	m_flagDown = fldn;
	name = nm;
}
unsigned short Input_Mouse::getFlagUp()
{
	return m_flagUp;
}
unsigned short Input_Mouse::getFlagDown()
{
	return m_flagDown;
}

void Input_Key::set(unsigned short vk, std::string nm)
{
	m_vKey = vk;
	name = nm;
}
unsigned short Input_Key::getVKey()
{
	return m_vKey;
}

void Input_Button::set(unsigned short mp, std::string nm)
{
	m_map = mp;
	name = nm;
}
unsigned short Input_Button::getMap()
{
	return m_map;
}

void MouseController::update(RAWMOUSE rmouse)
{
	bool relative = (rmouse.usFlags & MOUSE_MOVE_RELATIVE);

	X.catchState((float)rmouse.lLastX); // "Last" is relative position a.k.a velocity
	Y.catchState((float)rmouse.lLastY);

	if (rmouse.usButtonFlags & RI_MOUSE_WHEEL)
		Z.catchState((short)rmouse.usButtonData);
}
void MouseController::reset()
{
	X.update();
	Y.update();
	Z.update();
}

void KeysController::updateMouse(RAWMOUSE rmouse)
{
	USHORT flags = rmouse.usButtonFlags;

	for (unsigned char i = 0; i < m_mouseArray.size(); i++)
	{
		if (flags & m_mouseArray.at(i)->getFlagDown())
			m_mouseArray.at(i)->update(true);
		else if (flags & m_mouseArray.at(i)->getFlagUp())
			m_mouseArray.at(i)->update(false);
	}
}
void KeysController::updateKeyboard(RAWKEYBOARD rkeys)
{
	USHORT vk = rkeys.VKey;
	
	for (unsigned char i = 0; i < m_keyArray.size(); i++)
	{
		if (vk == m_keyArray.at(i)->getVKey())
		{
			if (rkeys.Message == WM_KEYDOWN)
				m_keyArray.at(i)->update(true);
			else if (rkeys.Message == WM_KEYUP)
				m_keyArray.at(i)->update(false);
		}
	}
}
void KeysController::reset()
{
	for (unsigned char i = 0; i < m_mouseArray.size(); i++)
	{
		m_mouseArray.at(i)->update(-1);
	}
	for (unsigned char i = 0; i < m_keyArray.size(); i++)
	{
		m_keyArray.at(i)->update(-1);
	}
}
void KeysController::setKey(Input_Key *key, unsigned short vk, std::string nm)
{
	key->set(vk, nm);
}
unsigned short KeysController::getKey(Input_Key *key)
{
	return key->getVKey();
}

void XInputController::update()
{
	ZeroMemory(&m_state, sizeof(XINPUT_STATE));
	enabled = false;

	if (XInputGetState(number, &m_state) == ERROR_SUCCESS) // controller is connected
		enabled = true;
	else
		return;

	if (enabled)
	{
		float rx = (float)m_state.Gamepad.sThumbRX / 32768;
		float ry = (float)m_state.Gamepad.sThumbRY / 32768;
		float lx = (float)m_state.Gamepad.sThumbLX / 32768;
		float ly = (float)m_state.Gamepad.sThumbLY / 32768;

		float mag_r = sqrt(rx * rx + ry * ry);
		float mag_l = sqrt(lx * lx + ly * ly);
		float dead_r = 0.25f;
		float dead_l = 0.25f;

		mag_r = min(mag_r, 1);
		mag_l = min(mag_l, 1);

		if (mag_r > dead_r)
		{
			float mult = (mag_r - dead_r) / (1.0f - dead_r);
			RX.catchState(rx * mult);
			RY.catchState(ry * mult);
		}
		else
		{
			RX.catchState(0);
			RY.catchState(0);
		}
		if (mag_l > dead_l)
		{
			float mult = (mag_l - dead_l) / (1.0f - dead_l);
			LX.catchState(lx * mult);
			LY.catchState(ly * mult);
		}
		else
		{
			LX.catchState(0);
			LY.catchState(0);
		}

		RX.update();
		RY.update();
		LX.update();
		LY.update();

		RZ = m_state.Gamepad.bRightTrigger;
		LZ = m_state.Gamepad.bLeftTrigger;
		if (RZ > 0)
			RT.update(true);
		else if (RT.getState() != BTN_UNPRESSED)
			RT.update(false);
		if (LZ > 0)
			LT.update(true);
		else if (LT.getState() != BTN_UNPRESSED)
			LT.update(false);

		for (unsigned char i = 0; i < m_btnArray.size(); i++)
		{
			if (m_btnArray.at(i)->getMap() != 0)
			{
				if (m_state.Gamepad.wButtons & m_btnArray.at(i)->getMap())
					m_btnArray.at(i)->update(true);
				else if (m_btnArray.at(i)->getState() != BTN_UNPRESSED)
					m_btnArray.at(i)->update(false);
			}			
		}
	}
}
void XInputController::reset()
{
	for (unsigned char i = 0; i < m_btnArray.size(); i++)
	{
		if (m_btnArray.at(i)->getMap() != 0)
			m_btnArray.at(i)->update(-1);
		else
		{
			m_btnArray.at(i)->update(-1);
		}
	}
}
void XInputController::vibrate(float leftmotor, float rightmotor)
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

///

HRESULT RegisterRID()
{
	WriteToConsole(L"Initializing RID objects... ");

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;				// mouse
	rid[0].dwFlags = RIDEV_NOLEGACY;
	rid[0].hwndTarget = window_main.hWnd;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;				// keyboard
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = window_main.hWnd;

	rid[2].usUsagePage = 0x01;
	rid[2].usUsage = 0x05;				// gamepad
	rid[2].dwFlags = 0;
	rid[2].hwndTarget = window_main.hWnd;

	rid[3].usUsagePage = 0x01;
	rid[3].usUsage = 0x04;				// joystick
	rid[3].dwFlags = 0;
	rid[3].hwndTarget = window_main.hWnd;

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
		keys.updateKeyboard(raw->data.keyboard);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		keys.updateMouse(raw->data.mouse);
		mouse.update(raw->data.mouse);
	}
	else if (raw->header.dwType == RIM_TYPEHID)
	{
		for (unsigned char i = 0; i< XUSER_MAX_COUNT; i++)
		{
			controller[i].update();
		}
	}

	delete[] lpb;

	return S_OK;
}