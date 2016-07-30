#ifndef GAMEFLOW_H
#define GAMEFLOW_H

#define GAMESTATE_LOADING_1 0x00000000
#define GAMESTATE_LOADING_2 0x00000001
#define GAMESTATE_LOADING_3 0x00000002
#define GAMESTATE_LOADING_4 0x00000003

#define GAMESTATE_SPLASH 0x00000010
#define GAMESTATE_MENU 0x00000011
#define GAMESTATE_INGAME 0x00000012
#define GAMESTATE_INVENTORY 0x00000013
#define GAMESTATE_CUTSCENE 0x00000014
#define GAMESTATE_FMV 0x00000015

extern unsigned int GameState;

void SetGameState(unsigned int state);
unsigned int GetGameState();
bool ForGameState(unsigned int state);

#endif
