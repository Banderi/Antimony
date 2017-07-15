#include "Antimony.h"

///

void Antimony::setGameState(unsigned int state)
{
	m_gameState = state;
}
unsigned int Antimony::getGameState()
{
	return m_gameState;
}
bool Antimony::ifGameState(unsigned int state)
{
	if (m_gameState == state)
		return true;
	else
		return false;
}

void Antimony::setSubSystem(unsigned char subs)
{
	m_subSystem = subs;
}
unsigned char Antimony::getSubSystem()
{
	return m_subSystem;
}
bool Antimony::ifSubSystem(unsigned char subs)
{
	if (m_subSystem == subs)
		return true;
	else
		return false;
}