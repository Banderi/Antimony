#include <string>

#include "Warnings.h"
#include "Input.h"
#include "Debug.h"
#include "Console.h"

#pragma comment(lib, "Xinput9_1_0.lib")

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
	if (m_pressState == BTN_DISABLED)
		return;
	if (down == 1)			// input is activated
	{
		if (m_pressState != BTN_HELD)
		{
			m_pressState = BTN_PRESSED;
			m_pressTime = clock();
		}
		else
			m_pressState = BTN_HELD;
	}
	else if (down == 0)		// input is released
	{
		m_pressState = BTN_RELEASED;
	}
	else if (down == -1)	// input does not change state
	{
		if (m_pressState == BTN_PRESSED)
		{
			m_pressState = BTN_HELD;
		}
		else if (m_pressState == BTN_RELEASED)
		{
			m_pressState = BTN_UNPRESSED;
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
void Input_Button::enable()
{
	m_pressState = BTN_UNPRESSED;
}
void Input_Button::disable()
{
	m_pressState = BTN_DISABLED;
}

void MouseController::update(RAWMOUSE rmouse)
{
	bool relative = (rmouse.usFlags & MOUSE_MOVE_RELATIVE);

	X.catchState((float)rmouse.lLastX); // "Last" is relative position a.k.a velocity
	Y.catchState((float)rmouse.lLastY);

	if (rmouse.usButtonFlags & RI_MOUSE_WHEEL)
		Z.catchState((short)rmouse.usButtonData);

	USHORT flags = rmouse.usButtonFlags;

	for (unsigned char i = 0; i < mouseArray.size(); i++)
	{
		if (flags & mouseArray.at(i)->getFlagDown())
			mouseArray.at(i)->update(true);
		else if (flags & mouseArray.at(i)->getFlagUp())
			mouseArray.at(i)->update(false);
	}
}
void MouseController::reset()
{
	for (unsigned char i = 0; i < mouseArray.size(); i++)
	{
		mouseArray.at(i)->update(-1);
	}

	X.update();
	Y.update();
	Z.update();
}
void MouseController::acquire(bool persistent)
{
	while (ShowCursor(false) >= 0);
	ClipCursor(&Antimony::window_main.plane);
	GetCursorPos(&m_fixedPos);
	if (persistent)
	{
		m_exclusive = true;
	}
}
void MouseController::release(bool persistent)
{
	while (ShowCursor(true) < 0);
	ClipCursor(NULL);
	if (persistent)
	{
		m_exclusive = false;
	}
}
bool MouseController::isExclusive()
{
	return m_exclusive;
}
void MouseController::clip(RECT r)
{
	m_clip = r;
}
void MouseController::resetCursor()
{
	SetCursorPos(m_fixedPos.x, m_fixedPos.y);
}
void MouseController::setCursor(unsigned int c)
{
	m_currentCursor = c;
}
void MouseController::show()
{
	m_visible = true;
}
void MouseController::hide()
{
	m_visible = false;
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

	static double life = clock();

	if (clock() - life > 10)
	{
		if (XInputGetState(m_number, &m_state) == ERROR_SUCCESS) // controller is connected
			enable();
		else
			disable();
		life = clock();
	}

	if (!isEnabled())
		return;
	else
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
	XInputSetState(m_number, &Vibration);
}
void XInputController::set(unsigned char n)
{
	m_number = n;
}
void XInputController::enable()
{
	if (isEnabled())
		return;
	else
	{
		m_enabled = true;

		for (unsigned char i = 0; i < m_btnArray.size(); i++)
		{
			m_btnArray.at(i)->enable();
		}
	}
}
void XInputController::disable()
{
	if (!isEnabled())
		return;
	else
	{
		m_enabled = false;

		for (unsigned char i = 0; i < m_btnArray.size(); i++)
		{
			m_btnArray.at(i)->disable();
		}
	}
}
bool XInputController::isEnabled()
{
	return m_enabled;
}

std::string GetKeyName(unsigned int k)
{
	switch (k)
	{
		case 0x01:
			return "Left Mouse Button";
		case 0x02:
			return "Right Mouse Button";
		case 0x03:
			return "Control-break";
		case 0x04:
			return "Middle Mouse Button";
		case 0x05:
			return "X1 Mouse Button";
		case 0x06:
			return "X2 Mouse Button";
		case 0x08:
			return "Backspace";
		case 0x09:
			return "Tab";
		case 0x0c:
			return "Clear";
		case 0x0d:
			return "Enter";
		case 0x13:
			return "Pause";
		case 0x14:
			return "Caps Lock";
		case 0x15:
			return "Kana";
		case 0x17:
			return "Junja";
		case 0x18:
			return "Final";
		case 0x19:
			return "Kanji";
		case 0x1b:
			return "Esc";
		case 0x1c:
			return "Convert";
		case 0x1d:
			return "Non Convert";
		case 0x1e:
			return "Accept";
		case 0x1f:
			return "Mode Change";
		case 0x20:
			return "Space";
		case 0x21:
			return "Page Up";
		case 0x22:
			return "Page Down";
		case 0x23:
			return "End";
		case 0x24:
			return "Home";
		case 0x25:
			return "Left";
		case 0x26:
			return "Up";
		case 0x27:
			return "Right";
		case 0x28:
			return "Down";
		case 0x29:
			return "Select";
		case 0x2a:
			return "Print";
		case 0x2b:
			return "Execute";
		case 0x2c:
			return "Print Screen";
		case 0x2d:
			return "Insert";
		case 0x2e:
			return "Delete";
		case 0x2f:
			return "Help";
		case 0x30:
			return "0";
		case 0x31:
			return "1";
		case 0x32:
			return "2";
		case 0x33:
			return "3";
		case 0x34:
			return "4";
		case 0x35:
			return "5";
		case 0x36:
			return "6";
		case 0x37:
			return "7";
		case 0x38:
			return "8";
		case 0x39:
			return "9";
		case 0x41:
			return "A";
		case 0x42:
			return "B";
		case 0x43:
			return "C";
		case 0x44:
			return "D";
		case 0x45:
			return "E";
		case 0x46:
			return "F";
		case 0x47:
			return "G";
		case 0x48:
			return "H";
		case 0x49:
			return "I";
		case 0x4a:
			return "J";
		case 0x4b:
			return "K";
		case 0x4c:
			return "L";
		case 0x4d:
			return "M";
		case 0x4e:
			return "N";
		case 0x4f:
			return "O";
		case 0x50:
			return "P";
		case 0x51:
			return "Q";
		case 0x52:
			return "R";
		case 0x53:
			return "S";
		case 0x54:
			return "T";
		case 0x55:
			return "U";
		case 0x56:
			return "V";
		case 0x57:
			return "W";
		case 0x58:
			return "X";
		case 0x59:
			return "Y";
		case 0x5a:
			return "Z";
		case 0x5b:
			return "Left Win";
		case 0x5c:
			return "Right Win";
		case 0x5d:
			return "Context";
		case 0x5f:
			return "Sleep";
		case 0x60:
			return "Numpad 0";
		case 0x61:
			return "Numpad 1";
		case 0x62:
			return "Numpad 2";
		case 0x63:
			return "Numpad 3";
		case 0x64:
			return "Numpad 4";
		case 0x65:
			return "Numpad 5";
		case 0x66:
			return "Numpad 6";
		case 0x67:
			return "Numpad 7";
		case 0x68:
			return "Numpad 8";
		case 0x69:
			return "Numpad 9";
		case 0x6a:
			return "Numpad *";
		case 0x6b:
			return "Numpad +";
		case 0x6c:
			return "Separator";
		case 0x6d:
			return "Numpad -";
		case 0x6e:
			return "Numpad .";
		case 0x6f:
			return "Numpad /";
		case 0x70:
			return "F1";
		case 0x71:
			return "F2";
		case 0x72:
			return "F3";
		case 0x73:
			return "F4";
		case 0x74:
			return "F5";
		case 0x75:
			return "F6";
		case 0x76:
			return "F7";
		case 0x77:
			return "F8";
		case 0x78:
			return "F9";
		case 0x79:
			return "F10";
		case 0x7a:
			return "F11";
		case 0x7b:
			return "F12";
		case 0x7c:
			return "F13";
		case 0x7d:
			return "F14";
		case 0x7e:
			return "F15";
		case 0x7f:
			return "F16";
		case 0x80:
			return "F17";
		case 0x81:
			return "F18";
		case 0x82:
			return "F19";
		case 0x83:
			return "F20";
		case 0x84:
			return "F21";
		case 0x85:
			return "F22";
		case 0x86:
			return "F23";
		case 0x87:
			return "F24";
		case 0x90:
			return "Num Lock";
		case 0x91:
			return "Scroll Lock";
		case 0x92:
			return "Jisho";
		case 0x93:
			return "Mashu";
		case 0x94:
			return "Touroku";
		case 0x95:
			return "Loya";
		case 0x96:
			return "Roya";
		case 0xa0:
			return "Left Shift";
		case 0xa1:
			return "Right Shift";
		case 0xa2:
			return "Left Ctrl";
		case 0xa3:
			return "Right Ctrl";
		case 0xa4:
			return "Left Alt";
		case 0xa5:
			return "Right Alt";
		case 0xa6:
			return "Browser Back";
		case 0xa7:
			return "Browser Forward";
		case 0xa8:
			return "Browser Refresh";
		case 0xa9:
			return "Browser Stop";
		case 0xaa:
			return "Browser Search";
		case 0xab:
			return "Browser Favorites";
		case 0xac:
			return "Browser Home";
		case 0xad:
			return "Volume Mute";
		case 0xae:
			return "Volume Down";
		case 0xaf:
			return "Volume Up";
		case 0xb0:
			return "Next Track";
		case 0xb1:
			return "Previous Track";
		case 0xb2:
			return "Stop";
		case 0xb3:
			return "Play / Pause";
		case 0xb4:
			return "Mail";
		case 0xb5:
			return "Media";
		case 0xb6:
			return "App 1";
		case 0xb7:
			return "App 2";
		case 0xba:
			return "OEM1 (:;)";
		case 0xbb:
			return "Plus (+=)";
		case 0xbc:
			return "Comma (<,)";
		case 0xbd:
			return "Minus (_-)";
		case 0xbe:
			return "Period (>.)";
		case 0xbf:
			return "OEM2 (?/)";
		case 0xc0:
			return "OEM3 (~`)";
		case 0xc1:
			return "Abnt C1";
		case 0xc2:
			return "Abnt C2";
		case 0xdb:
			return "OEM4 ({[)";
		case 0xdc:
			return "OEM5 (|\)";
		case 0xdd:
			return "OEM6 (}])";
		case 0xde:
			return "OEM7 (\"')";
		case 0xdf:
			return "OEM8 (§!)";
		case 0xe1:
			return "Ax";
		case 0xe2:
			return "OEM102 (><)";
		case 0xe3:
			return "IcoHlp";
		case 0xe4:
			return "Ico00";
		case 0xe5:
			return "Process";
		case 0xe6:
			return "IcoClr";
		case 0xe7:
			return "Packet";
		case 0xe9:
			return "Reset";
		case 0xea:
			return "Jump";
		case 0xeb:
			return "OemPa1";
		case 0xec:
			return "OemPa2";
		case 0xed:
			return "OemPa3";
		case 0xee:
			return "WsCtrl";
		case 0xef:
			return "Cu Sel";
		case 0xf0:
			return "OemAttn";
		case 0xf1:
			return "Finish";
		case 0xf2:
			return "Copy";
		case 0xf3:
			return "Auto";
		case 0xf4:
			return "Enlw";
		case 0xf5:
			return "Back Tab";
		case 0xf6:
			return "Attn";
		case 0xf7:
			return "Cr Sel";
		case 0xf8:
			return "Ex Sel";
		case 0xf9:
			return "Er Eof";
		case 0xfa:
			return "Play";
		case 0xfb:
			return "Zoom";
		case 0xfc:
			return "NoName";
		case 0xfd:
			return "Pa1";
		case 0xfe:
			return "OemClr";
		case 0xff:
			return "N/A";
		default:
			return "???";
	}
}

///

HRESULT Antimony::registerRID()
{
	log(L"Initializing RID objects...", CSL_SYSTEM);

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
		logError(HRESULT_FROM_WIN32(GetLastError()));
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		XINPUT_STATE state;
		for (unsigned char i = 0; i < XUSER_MAX_COUNT; i++)
		{
			controller[i].disable();
			controller[i].set(i);
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			if (XInputGetState(i, &state) == ERROR_SUCCESS) // controller is connected
				controller[i].enable();
		}

		log(L" done!\n", CSL_SUCCESS, false);
		return S_OK;
	}
}
HRESULT Antimony::handleInput(MSG *msg)
{
	return handleInput(window_main.hWnd, msg->message, msg->wParam, msg->lParam);
}
HRESULT Antimony::handleInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	wm_message = true;

	switch (message)
	{
		case WM_KEYDOWN:
		{
			wm_keydown = true;
			return S_OK;
		}
		case WM_CHAR:
		{
			if (devConsole.isOpen())
				devConsole.parse(message, wParam, lParam, &controls);
			return S_OK;
		}
		case WM_INPUT:
		{
			SetActiveWindow(window_main.hWnd);
			if (mouse.isExclusive())
				mouse.resetCursor();
			wm_input = true;
			UINT dwSize;

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			LPBYTE lpb = new BYTE[dwSize];
			if (lpb == NULL)
				return 0;

			int readSize = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
			if (readSize != dwSize)
				return HRESULT_FROM_WIN32(GetLastError());

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				keys.updateKeyboard(raw->data.keyboard);
			}
			else if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				mouse.update(raw->data.mouse);
			}
			else if (raw->header.dwType == RIM_TYPEHID)
			{
				for (unsigned char i = 0; i < XUSER_MAX_COUNT; i++)
				{
					controller[i].update();
				}
			}

			delete[] lpb;
			return S_OK;
		}
		default:
			return S_OK;
	}
}

namespace Antimony
{
	RAWINPUTDEVICE rid[4];

	MouseController mouse;
	KeysController keys;
	XInputController controller[4];

	bool wm_message = false;
	bool wm_input = false;
	bool wm_keydown = false;

	HCURSOR arrow = LoadCursorW(NULL, IDC_ARROW);
	HCURSOR beam = LoadCursorW(NULL, IDC_IBEAM);
	HCURSOR hand = LoadCursorW(NULL, IDC_HAND);
	HCURSOR busy = LoadCursorW(NULL, IDC_WAIT);
}