#include <string>
#include <ctime>

#include "Warnings.h"
#include "Input.h"
#include "Window.h"
#include "DebugWin.h"

RAWINPUTDEVICE rid[4];

Mouse mouse;
Keys keys;

//

void Mouse::Update(RAWMOUSE rmouse)
{
	oldx = posx;
	oldy = posy;
	oldz = posz;

	velx = rmouse.lLastX; // "Last" is relative position a.k.a velocity
	vely = rmouse.lLastY;
	if (rmouse.usButtonFlags & RI_MOUSE_WHEEL)
		velz = (short)rmouse.usButtonData;

	posx = oldx + velx;
	posy = oldy + vely;
	posz = oldz + velz;

	UpdateButtonState(rmouse.usButtonFlags);
}
void Mouse::UpdateButtonState(unsigned short flags)
{
	if (flags & RI_MOUSE_LEFT_BUTTON_DOWN)
		left = 1;
	if (flags & RI_MOUSE_LEFT_BUTTON_UP)
		left = 3;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
		middle = 1;
	if (flags & RI_MOUSE_MIDDLE_BUTTON_UP)
		middle = 3;
	if (flags & RI_MOUSE_RIGHT_BUTTON_DOWN)
		right = 1;
	if (flags & RI_MOUSE_RIGHT_BUTTON_UP)
		right = 3;
}
int Mouse::GetButtonState(unsigned int button)
{
	switch (button)
	{
		case 0:
			return left;
		case 1:
			return middle;
		case 2:
			return right;
		default:
			return -1;
	}
}
MouseCoordinate Mouse::GetCoord(unsigned int coord)
{
	switch (coord)
	{
	case 0:
		return MouseCoordinate(posx, oldx, velx);
	case 1:
		return MouseCoordinate(posy, oldy, vely);
	case 2:
		return MouseCoordinate(posz, oldz, velz);
	default:
		return MouseCoordinate(-1, -1, -1);
	}
}
void Mouse::Reset()
{
	oldx = posx;
	oldy = posy;
	oldz = posz;

	velx = 0;
	vely = 0;
	velz = 0;

	switch (left)
	{
		case 1: // pressed
		{
			left = 2;
			break;
		}
		case 3: // released
		{
			left = 0;
			break;
		}
		default:
			break;
	}

	switch (middle)
	{
		case 1: // pressed
		{
			middle = 2;
			break;
		}
		case 3: // released
		{
			middle = 0;
			break;
		}
		default:
			break;
	}

	switch (right)
	{
		case 1: // pressed
		{
			right = 2;
			break;
		}
		case 3: // released
		{
			right = 0;
			break;
		}
		default:
			break;
	}
}
Mouse::Mouse()
{
	left = 0;
	middle = 0;
	right = 0;

	posx = 0;
	posy = 0;
	posz = 0;
	oldx = 0;
	oldy = 0;
	oldz = 0;
	velx = 0;
	vely = 0;
	velz = 0;

	exclusive = 1;
	reset = 0;
}

void Keys::Update(RAWKEYBOARD rkeys)
{
	//vec3 mov = vec3( 0, 0, 0 );
	//float theta = camera.getAngle(th);

	if (rkeys.Message == WM_KEYDOWN)
	{
		if (rkeys.VKey == forward.vkey)
			UpdateKeyPress(&forward, true);
		if (rkeys.VKey == backward.vkey)
			UpdateKeyPress(&backward, true);
		if (rkeys.VKey == left.vkey)
			UpdateKeyPress(&left, true);
		if (rkeys.VKey == right.vkey)
			UpdateKeyPress(&right, true);
	}
	else if (rkeys.Message == WM_KEYUP)
	{
		if (rkeys.VKey == forward.vkey)
			UpdateKeyPress(&forward, false);
		if (rkeys.VKey == backward.vkey)
			UpdateKeyPress(&backward, false);
		if (rkeys.VKey == left.vkey)
			UpdateKeyPress(&left, false);
		if (rkeys.VKey == right.vkey)
			UpdateKeyPress(&right, false);
	}

	/*if (rkeys.VKey == forward.vkey)
		mov = mov + vec3(cosf(theta), 0, sinf(theta));
	if (rkeys.VKey == backward.vkey)
		mov = mov + vec3(-cosf(theta), 0, -sinf(theta));
	if (rkeys.VKey == left.vkey)
		mov = mov + vec3(-sinf(theta), 0, cosf(theta));
	if (rkeys.VKey == right.vkey)
		mov = mov + vec3(sinf(theta), 0, -cosf(theta));*/

	//player.moveToPoint(player.getPos() + mov * 0.25, -1);
}
void Keys::UpdateKeyPress(KEY* key, char down)
{
	if (down == 1)
	{
		key->press = 1;
		key->time = clock();
	}
	else if (down == 0)
	{
		key->press = 3;
	}
	else if (down == -1)
	{
		if (key->press == 1)
			key->press = 2;
		else if (key->press == 3)
		{
			key->press = 0;
			key->time = -1;
		}
	}
}
void Keys::Reset()
{
	//KEY* key = &forward;
	//switch (key->press)
	//{
	//	case 1:
	//	{
	//		key->press = 2;
	//		break;
	//	}
	//	case 3:
	//	{
	//		key->press = 0;
	//		key->time = -1;
	//		break;
	//	}
	//	default:
	//		break;
	//}

	UpdateKeyPress(&forward, -1);
	UpdateKeyPress(&backward, -1);
	UpdateKeyPress(&left, -1);
	UpdateKeyPress(&right, -1);
}
Keys::Keys()
{
	exclusive = 0;
}

//

HRESULT RegisterRID()
{
	WriteToConsole(L"Initializing RID objects... ");

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x02;				// HID mouse
	rid[0].dwFlags = RIDEV_NOLEGACY;
	rid[0].hwndTarget = hWnd;

	rid[1].usUsagePage = 0x01;
	rid[1].usUsage = 0x06;				// HID keyboard
	rid[1].dwFlags = 0;
	rid[1].hwndTarget = hWnd;

	rid[2].usUsagePage = 0x01;
	rid[2].usUsage = 0x05;				// gamepad
	rid[2].dwFlags = 0;
	rid[2].hwndTarget = hWnd;	

	if (RegisterRawInputDevices(rid, 3, sizeof(rid[0])) == false)
	{
		LogError(HRESULT_FROM_WIN32(GetLastError()));
		return HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
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
		keys.Update(raw->data.keyboard);
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		mouse.Update(raw->data.mouse);
	}

	delete[] lpb;

	return S_OK;
}