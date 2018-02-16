#pragma once

///

#define GAMESTATE_LOADING_1		0x00
#define GAMESTATE_LOADING_2		0x01
#define GAMESTATE_LOADING_3		0x02
#define GAMESTATE_LOADING_4		0x03
#define GAMESTATE_SPLASH		0x04

#define GAMESTATE_MAINMENU		0x10
#define GAMESTATE_INGAME		0x11
#define GAMESTATE_PAUSEMENU		0x13
#define GAMESTATE_INVENTORY		0x14
#define GAMESTATE_PAUSED		0x15

#define GAMESTATE_CUTSCENE		0x20
#define GAMESTATE_FMV			0x21
#define GAMESTATE_CREDITS		0x23

///

namespace Antimony
{
	extern unsigned int gameState;

	void setGameState(unsigned int state);
	unsigned int getGameState();
	bool ifGameState(unsigned int state);
}