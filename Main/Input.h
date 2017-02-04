#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <ctime>
#include <string>
#include <Xinput.h>
#include <vector>

///

#define MOUSE_LEFT 0
#define MOUSE_MIDDLE 1
#define MOUSE_RIGHT 2

#define BTN_UNPRESSED 0
#define BTN_PRESSED 1
#define BTN_HELD 2
#define BTN_RELEASED 3

#define COORD_X 0
#define COORD_Y 1
#define COORD_Z 2

#define KEY_TYPE_KEYBOARD 0x00
#define KEY_TYPE_MOUSE 0x01
#define KEY_TYPE_CONTROLLER 0x02

#define CONTROLLER_1 0x00
#define CONTROLLER_2 0x01
#define CONTROLLER_3 0x02
#define CONTROLLER_4 0x03

///

extern RAWINPUTDEVICE rid[4];

///

struct Axis
{
protected:
	float m_pos, m_oldPos, m_vel;

public:
	void catchState(float v);
	void update();
	float getPos();
	float getVel();

	Axis(float _a, float _b, float _c)
	{
		m_pos = _a;
		m_oldPos = _b;
		m_vel = _c;
	}
	Axis()
	{
		m_pos = 0;
		m_oldPos = 0;
		m_vel = 0;
	}
};
struct Input
{
protected:
	unsigned char m_pressState;
	unsigned char m_handleType;
	clock_t m_pressTime;

public:
	std::string name;

	void update(char down);
	unsigned char getState();
	float getTime();
	void set(std::string nm);
};
struct Input_Mouse : Input
{
protected:
	unsigned short m_flagUp;
	unsigned short m_flagDown;

public:
	void set(unsigned short flup, unsigned short fldn, std::string nm);

	unsigned short getFlagUp();
	unsigned short getFlagDown();

	Input_Mouse()
	{
		m_flagUp = 0x00;
		m_flagDown = 0x00;
		m_pressState = BTN_RELEASED;
		m_pressTime = -1;
		name = "";
		m_handleType = KEY_TYPE_MOUSE;
	}
};
struct Input_Key : Input
{
protected:
	unsigned short m_vKey;	

public:
	void set(unsigned short vk, std::string nm);
	unsigned short getVKey();

	Input_Key()
	{
		m_vKey = 0x00;
		m_pressState = BTN_RELEASED;
		m_handleType = -1;
		name = "";
		m_handleType = KEY_TYPE_KEYBOARD;
	}
};
struct Input_Button : Input
{
protected:
	unsigned short m_map;

public:
	void set(unsigned short mp, std::string nm);
	unsigned short getMap();

	Input_Button()
	{
		m_map = 0x00;
		m_pressState = BTN_RELEASED;
		m_pressTime = -1;
		name = "";
		m_handleType = KEY_TYPE_CONTROLLER;
	}
};

class MouseController
{
private:
	bool m_reset;

public:
	Axis X;
	Axis Y;
	Axis Z;

	bool exclusive;

	void update(RAWMOUSE rmouse);
	void reset();

	MouseController()
	{
		exclusive = 1;
		m_reset = 0;
	}
};
class KeysController
{
private:
	std::vector<Input_Mouse*> m_mouseArray;
	std::vector<Input_Key*> m_keyArray;

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

	Input_Key sk_enter;
	Input_Key sk_escape;
	Input_Key sk_shift;
	Input_Key sk_ctrl;
	Input_Key sk_alt;
	Input_Key sk_space;

	Input_Key sk_arrUp;
	Input_Key sk_arrDown;
	Input_Key sk_arrLeft;
	Input_Key sk_arrRight;

	bool exclusive;

	//

	void updateMouse(RAWMOUSE rmouse);
	void updateKeyboard(RAWKEYBOARD rkeys);
	void reset();
	void setKey(Input_Key *key, unsigned short vk, std::string nm);
	unsigned short getKey(Input_Key *key);

	KeysController()
	{
		exclusive = 0;

		LMB.set(RI_MOUSE_LEFT_BUTTON_UP, RI_MOUSE_LEFT_BUTTON_DOWN, "Left Mouse Button");
		MMB.set(RI_MOUSE_MIDDLE_BUTTON_UP, RI_MOUSE_MIDDLE_BUTTON_DOWN, "Middle Mouse Button");
		RMB.set(RI_MOUSE_RIGHT_BUTTON_UP, RI_MOUSE_RIGHT_BUTTON_DOWN, "Right Mouse Button");

		m_mouseArray.push_back(&LMB);
		m_mouseArray.push_back(&MMB);
		m_mouseArray.push_back(&RMB);

		m_keyArray.push_back(&sk_enter);
		m_keyArray.push_back(&sk_escape);
		m_keyArray.push_back(&sk_shift);
		m_keyArray.push_back(&sk_ctrl);
		m_keyArray.push_back(&sk_alt);
		m_keyArray.push_back(&sk_space);

		m_keyArray.push_back(&sk_arrUp);
		m_keyArray.push_back(&sk_arrDown);
		m_keyArray.push_back(&sk_arrLeft);
		m_keyArray.push_back(&sk_arrRight);

		m_keyArray.push_back(&forward);
		m_keyArray.push_back(&backward);
		m_keyArray.push_back(&left);
		m_keyArray.push_back(&right);
		m_keyArray.push_back(&sprint);
		m_keyArray.push_back(&jump);
		m_keyArray.push_back(&action);

		sk_enter.set(0x0d, "Enter");
		sk_escape.set(0x1b, "Escape");
		sk_shift.set(0xa0, "Shift");
		sk_ctrl.set(0xa2, "Ctrl");
		sk_alt.set(0xa4, "Alt");
		sk_space.set(0x20, "Space");
	}
};
class XInputController
{
private:
	XINPUT_STATE m_state;
	std::vector<Input_Button*> m_btnArray;

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

	void update();
	void reset();
	void vibrate(float leftmotor = 0.0f, float rightmotor = 0.0f);

	XInputController()
	{
		ZeroMemory(&m_state, sizeof(XINPUT_STATE));
		enabled = false;

		LT.set(0, "Left Trigger/L2");
		RT.set(0, "Right Trigger/R2");
		LB.set(XINPUT_GAMEPAD_LEFT_SHOULDER, "Left Bumper/L1");
		RB.set(XINPUT_GAMEPAD_RIGHT_SHOULDER, "Right Bumper/R1");
		LS.set(XINPUT_GAMEPAD_LEFT_THUMB, "Left Stick/L3");
		RS.set(XINPUT_GAMEPAD_RIGHT_THUMB, "Right Stick/R3");
		A.set(XINPUT_GAMEPAD_A, "A/Cross");
		B.set(XINPUT_GAMEPAD_B, "B/Circle");
		X.set(XINPUT_GAMEPAD_X, "X/Square");
		Y.set(XINPUT_GAMEPAD_Y, "Y/Triangle");
		Up.set(XINPUT_GAMEPAD_DPAD_UP, "D-Pad Up");
		Down.set(XINPUT_GAMEPAD_DPAD_DOWN, "D-Pad Down");
		Left.set(XINPUT_GAMEPAD_DPAD_LEFT, "D-Pad Left");
		Right.set(XINPUT_GAMEPAD_DPAD_RIGHT, "D-Pad Right");
		Start.set(XINPUT_GAMEPAD_START, "Start");
		Back.set(XINPUT_GAMEPAD_BACK, "Back/Select");

		m_btnArray.push_back(&LT);
		m_btnArray.push_back(&RT);
		m_btnArray.push_back(&LB);
		m_btnArray.push_back(&RB);
		m_btnArray.push_back(&LS);
		m_btnArray.push_back(&RS);
		m_btnArray.push_back(&A);
		m_btnArray.push_back(&B);
		m_btnArray.push_back(&X);
		m_btnArray.push_back(&Y);
		m_btnArray.push_back(&Up);
		m_btnArray.push_back(&Down);
		m_btnArray.push_back(&Left);
		m_btnArray.push_back(&Right);
		m_btnArray.push_back(&Start);
		m_btnArray.push_back(&Back);
	}
};

extern MouseController mouse;
extern KeysController keys;
extern XInputController controller[4];

///

HRESULT RegisterRID();
HRESULT HandleRaw(MSG msg);