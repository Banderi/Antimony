#include "Gameflow.h"

///

unsigned int Antimony::gameState;

void Antimony::setGameState(unsigned int state)
{
	gameState = state;
}
unsigned int Antimony::getGameState()
{
	return gameState;
}
bool Antimony::ifGameState(unsigned int state)
{
	if (gameState == state)
		return true;
	else
		return false;
}