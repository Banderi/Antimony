#include "Gameflow.h"

///

unsigned int GameState;
unsigned char SubSystem;

///

void SetGameState(unsigned int state)
{
	GameState = state;
}
unsigned int GetGameState()
{
	return GameState;
}
bool IfGameState(unsigned int state)
{
	if (GameState == state)
		return true;
	else
		return false;
}

void SetSubSystem(unsigned char subs)
{
	SubSystem = subs;
}
unsigned char GetSubSystem()
{
	return SubSystem;
}
bool IfSubSystem(unsigned char subs)
{
	if (SubSystem == subs)
		return true;
	else
		return false;
}