#pragma once

///

#define GAMESTATE_LOADING_1		0x00
#define GAMESTATE_LOADING_2		0x01
#define GAMESTATE_LOADING_3		0x02
#define GAMESTATE_LOADING_4		0x03

#define GAMESTATE_SPLASH		0x10
#define GAMESTATE_MENU			0x11
#define GAMESTATE_INGAME		0x12
#define GAMESTATE_PAUSE			0x13
#define GAMESTATE_INVENTORY		0x14
#define GAMESTATE_CUTSCENE		0x15
#define GAMESTATE_FMV			0x16


#define SUBSYS_FPS_TPS			0x00

#define SUBSYS_RTS				0x10
#define SUBSYS_TILED2D			0x11
#define SUBSYS_TILED3D			0x12

#define SUBSYS_SIDE2D			0x20
#define SUBSYS_SIDE3D			0x21

#define SUBSYS_VISUAL			0x30
#define SUBSYS_FISHEYE			0x31

///

extern unsigned int GameState;
extern unsigned char SubSystem;

///

void SetGameState(unsigned int state);
unsigned int GetGameState();
bool IfGameState(unsigned int state);

void SetSubSystem(unsigned char subs);
unsigned char GetSubSystem();
bool IfSubSystem(unsigned char subs);