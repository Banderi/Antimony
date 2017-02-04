#pragma once

#include <Windows.h>

///

struct WindowParams
{
	HWND hWnd;								// The main window handle
	int width, height, x, y;				// The main window position and size relative to the screen

	float top;								// Window top edge's position from center (-1/2 height)
	float bottom;							// Window bottom edge's position from center (1/2 height)
	float right;							// Window right edge's position from center (1/2 width)
	float left;								// Window left edge's position from center (-1/2 width)

	RECT plane;								// `tagRECT` object containing the window surface
	bool fullscreen;						//
	bool borderless;						// Defines the window's style (true = borderless window)
	float aspect;							// Window aspect ratio
};

struct DisplayParams
{
	unsigned int gpu_num, gpu_denom;		// Video card numerator/denominator parameters
	int gpu_vram;							// Video card memory amount
	char gpu_desc[128];						// Video card description

	char filtering;							// Texture filtering
	bool vsync;								// Vertical sync
	bool triple_buff;						// Triple buffering
};

struct AudioParams
{
	// TODO: Implement audio
};

struct GameParams
{
	bool debug;								// Debugging features
	bool cheats;							// sv_cheats 1
	char difficulty;						// Game difficulty
	bool camera_friction;					// Responsiveness of camera movement

	bool dbg_wireframe;						// Bullet debugging wireframe rendering
	int dbg_entityfollow;					// Entity followed by camera (for debugging)
};

struct ControlParams
{
	float m_sensitivity;					// Mouse sensitivity
	float x_sensitivity;					// Controller sensitivity

	bool m_invertxaxis;						// Mouse X axis inverted yes/no
	bool m_invertyaxis;						// Mouse Y axis inverted yes/no
	bool x_invertxaxis;						// Controller X axis inverted yes/no
	bool x_invertyaxis;						// Controller Y axis inverted yes/no

	unsigned int k_forward;					//
	unsigned int k_backward;				//
	unsigned int k_left;					//
	unsigned int k_right;					//

	unsigned int k_sprint;					//
	unsigned int k_jump;					//
	unsigned int k_action;					//
};

extern WindowParams window_main;
extern DisplayParams display;
extern AudioParams audio;
extern GameParams game;
extern ControlParams controls;