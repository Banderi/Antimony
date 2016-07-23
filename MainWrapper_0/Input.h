#ifndef INPUT_H
#define INPUT_H

#include <Windows.h>
#include <Windowsx.h>
#include <string>

extern RAWINPUTDEVICE rid[4];

struct MouseCoordinate
{
	int pos, oldpos, vel;
	MouseCoordinate(int _a, int _b, int _c) { pos = _a; oldpos = _b; vel = _c; };
};
struct KEY
{
	unsigned short vkey;
	unsigned char press;
	float time;
	std::string name;

	KEY()
	{
		vkey = 0x0;
		press = 0;
		time = -1;
		name = "";
	};
};

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

class Mouse
{
	unsigned char left;
	unsigned char middle;
	unsigned char right;

	int posx, oldx, velx;
	int posy, oldy, vely;
	int posz, oldz, velz;

	bool exclusive;
	bool reset;

public:
	void Update(RAWMOUSE rmouse);
	void UpdateButtonState(unsigned short flags);
	int GetButtonState(unsigned int button);
	MouseCoordinate GetCoord(unsigned int coord);
	void Reset();

	Mouse();
};
extern Mouse mouse;

class Keys
{
	bool exclusive;

public:
	KEY forward;
	KEY backward;
	KEY left;
	KEY right;

	KEY sprint;
	KEY jump;
	KEY action;

	void Update(RAWKEYBOARD rkeys);
	void UpdateKeyPress(USHORT vk, KEY *key, char down);
	void Reset();
	void SetKey(KEY *key, unsigned short vk);
	unsigned short GetKey(KEY *key);

	Keys();
};
extern Keys keys;

//

HRESULT RegisterRID();
HRESULT HandleRaw(MSG msg);

#endif