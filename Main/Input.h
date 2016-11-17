#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <ctime>
#include <string>
#include <Xinput.h>

#define leftbutton 0
#define middlebutton 1
#define rightbutton 2

#define unpressed 0
#define pressed 1
#define held 2
#define released 3

#define xcoord 0
#define ycoord 1
#define zcoord 2

#define KEY_TYPE_KEYBOARD 0x00
#define KEY_TYPE_MOUSE 0x01
#define KEY_TYPE_CONTROLLER 0x02

#define MOUSE_COUNT 3
#define KEY_COUNT 7
#define BUTTON_COUNT 16

extern RAWINPUTDEVICE rid[4];

struct Axis
{
	float pos, oldpos, vel;

	void Update(float v);
	Axis(float _a, float _b, float _c)
	{
		pos = _a;
		oldpos = _b;
		vel = _c;
	}

	Axis()
	{
		pos = 0;
		oldpos = 0;
		vel = 0;
	}
};
struct Input
{
protected:
	unsigned char press;
	unsigned char handletype;
	clock_t time;

public:
	std::string name;

	void Update(char down);
	unsigned char GetState();
	float GetTime();
	void Set(std::string nm);
};
struct Input_Mouse : Input
{
public:
	unsigned short flagup;
	unsigned short flagdown;

	Input_Mouse()
	{
		flagup = 0x00;
		flagdown = 0x00;
		press = 0;
		time = -1;
		name = "";
		handletype = KEY_TYPE_MOUSE;
	}

public:
	void Set(unsigned short flup, unsigned short fldn, std::string nm);
};
struct Input_Key : Input
{
public:
	unsigned short vkey;	

	Input_Key()
	{
		vkey = 0x00;
		press = 0;
		time = -1;
		name = "";
		handletype = KEY_TYPE_KEYBOARD;
	}

public:
	void Set(unsigned short vk, std::string nm);	
};
struct Input_Button : Input
{
public:
	unsigned short map;

	Input_Button()
	{
		map = 0x00;
		press = 0;
		time = -1;
		name = "";
		handletype = KEY_TYPE_CONTROLLER;
	}

public:
	void Set(unsigned short mp, std::string nm);
};

//

class MouseController
{
private:
	bool reset;

public:
	Axis X;
	Axis Y;
	Axis Z;

	bool exclusive;

	void Update(RAWMOUSE rmouse);
	void Reset();

	MouseController()
	{
		exclusive = 1;
		reset = 0;
	}
};
class KeysController
{
private:
	Input_Mouse* mousearray[MOUSE_COUNT];
	Input_Key* keyarray[KEY_COUNT];

public:
	Input_Mouse LMB;
	Input_Mouse MMB;
	Input_Mouse RMB;

	Input_Key forward;
	Input_Key backward;
	Input_Key left;
	Input_Key right;
	Input_Key sprint;
	Input_Key jump;
	Input_Key action;

	bool exclusive;

	//

	void UpdateMouse(RAWMOUSE rmouse);
	void UpdateKeyboard(RAWKEYBOARD rkeys);
	void Reset();
	void SetKey(Input_Key *key, unsigned short vk);
	unsigned short GetKey(Input_Key *key);

	KeysController()
	{
		exclusive = 0;

		LMB.Set(RI_MOUSE_LEFT_BUTTON_UP, RI_MOUSE_LEFT_BUTTON_DOWN, "Left Mouse Button");
		MMB.Set(RI_MOUSE_MIDDLE_BUTTON_UP, RI_MOUSE_MIDDLE_BUTTON_DOWN, "Middle Mouse Button");
		RMB.Set(RI_MOUSE_RIGHT_BUTTON_UP, RI_MOUSE_RIGHT_BUTTON_DOWN, "Right Mouse Button");

		mousearray[0] = &LMB;
		mousearray[1] = &MMB;
		mousearray[2] = &RMB;

		keyarray[0] = &forward;
		keyarray[1] = &backward;
		keyarray[2] = &left;
		keyarray[3] = &right;
		keyarray[4] = &sprint;
		keyarray[5] = &jump;
		keyarray[6] = &action;
	}
};
class XInputController
{
private:
	XINPUT_STATE state;
	Input_Button* btnarray[BUTTON_COUNT];

public:
	Axis RX, LX;
	Axis RY, LY;
	BYTE RZ, LZ;

	Input_Button LT;
	Input_Button RT;
	Input_Button LB;
	Input_Button RB;
	Input_Button LS;
	Input_Button RS;

	Input_Button A;
	Input_Button B;
	Input_Button X;
	Input_Button Y;

	Input_Button Up;
	Input_Button Down;
	Input_Button Left;
	Input_Button Right;

	Input_Button Start;
	Input_Button Back;
	
	unsigned char number;
	bool enabled;

	//

	void Update();
	void Reset();
	void Vibrate(float leftmotor = 0.0f, float rightmotor = 0.0f);

	XInputController()
	{
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		enabled = false;

		LT.Set(0, "Left Trigger/L2");
		RT.Set(0, "Right Trigger/R2");
		LB.Set(XINPUT_GAMEPAD_LEFT_SHOULDER, "Left Bumper/L1");
		RB.Set(XINPUT_GAMEPAD_RIGHT_SHOULDER, "Right Bumper/R1");
		LS.Set(XINPUT_GAMEPAD_LEFT_THUMB, "Left Stick/L3");
		RS.Set(XINPUT_GAMEPAD_RIGHT_THUMB, "Right Stick/R3");
		A.Set(XINPUT_GAMEPAD_A, "A/Cross");
		B.Set(XINPUT_GAMEPAD_B, "B/Circle");
		X.Set(XINPUT_GAMEPAD_X, "X/Square");
		Y.Set(XINPUT_GAMEPAD_Y, "Y/Triangle");
		Up.Set(XINPUT_GAMEPAD_DPAD_UP, "D-Pad Up");
		Down.Set(XINPUT_GAMEPAD_DPAD_DOWN, "D-Pad Down");
		Left.Set(XINPUT_GAMEPAD_DPAD_LEFT, "D-Pad Left");
		Right.Set(XINPUT_GAMEPAD_DPAD_RIGHT, "D-Pad Right");
		Start.Set(XINPUT_GAMEPAD_START, "Start");
		Back.Set(XINPUT_GAMEPAD_BACK, "Back/Select");

		btnarray[0] = &LT;
		btnarray[1] = &RT;
		btnarray[2] = &LB;
		btnarray[3] = &RB;
		btnarray[4] = &LS;
		btnarray[5] = &RS;
		btnarray[6] = &A;
		btnarray[7] = &B;
		btnarray[8] = &X;
		btnarray[9] = &Y;
		btnarray[10] = &Up;
		btnarray[11] = &Down;
		btnarray[12] = &Left;
		btnarray[13] = &Right;
		btnarray[14] = &Start;
		btnarray[15] = &Back;
	}
};

extern MouseController mouse;
extern KeysController keys;
extern XInputController controller[4];

//

HRESULT RegisterRID();
HRESULT HandleRaw(MSG msg);