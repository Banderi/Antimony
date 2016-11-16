#ifndef PARAM_H
#define PARAM_H

#include <Windows.h>

struct windowParams
{
	HWND hWnd;
	int width, height, X, Y;
	RECT plane;
	bool fullscreen;
	bool borderless;
	float aspect;
};

struct displayParams
{
	unsigned int gpu_num, gpu_denom;		// video card numerator/denominator parameters
	int gpu_vram;							// video card memory
	char gpu_desc[128];						// video card description

	char filtering;							// texture filtering
	bool v_sync;							// vertical sync
	bool triple_buff;						// triple buffering
};

struct audioParams
{
	// TODO: Implement audio
};

struct gameParams
{
	bool debug;
	bool cheats;
	char difficulty;
	bool camera_friction;
};

extern windowParams windowMain;
extern displayParams display;
extern audioParams audio;
extern gameParams game;

#endif