#pragma once

#include <Windows.h>

///

#define AA_SSAA		0			// Supersampling AA/Full-scene AA		- plain supersampling; most performance intensive, highest quality

#define AA_MSAA		1			// Multisample AA						- shared samples between pixels; less performance intensive than SSAA, high quality

#define AA_CSAA		2			// (Nvidia) Coverage Sample AA			- increased coverage-samples with same number of color samples
#define AA_EQAA		3			// (AMD) Enhanced Quality AA			- basically same as CSAA

#define AA_MLAA		4			// (AMD) Morphological AA				- (post-processing) contrast blur filter
#define AA_FXAA		5			// (Nvidia) Fast Approximate AA			- (post-processing) faster than MLAA, slightly better quality

#define AA_SMAA		6			// (Crytek) Enhanced Subpixel Morph. AA	- (post-processing) mixed mode based on MLAA/FXAA somewhat closer to MSAA, better quality

#define AA_TXAA		7			// (Nvidia) Temporal AA					- ???
#define AA_TAA		8			// (Bethesda) Temporal AA				- ???
#define AA_TSSAA	9			// (Id Software) Temporal SSAA			- ???

#define AA_2X		2
#define AA_4X		4
#define AA_8X		8
#define AA_16X		16
#define AA_32X		32
#define AA_64X		64

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
	wchar_t gpu_desc[128];					// Video card description

	int width, height;						// The device resolution
	float top;								// Window top edge's position from center (-1/2 height)
	float bottom;							// Window bottom edge's position from center (1/2 height)
	float right;							// Window right edge's position from center (1/2 width)
	float left;								// Window left edge's position from center (-1/2 width)

	bool vsync;								// Vertical sync
	bool triple_buff;						// Triple buffering

	char tex_filtering;						// Texture filtering
	char tex_mipmap;						// Texture mipmap quality
	char tex_quality;						// Texture quality
	char aa_technique;						// Anti-aliasing technique
	int aa_resolution;						// Anti-aliasing resolution
	char ao_technique;						// Ambient occlusion technique
	int ao_resolution;						// Ambient occlusion resolution
	char sh_technique;						// Shadows technique
	int sh_resolution;						// Shadows resolution

	char reflect;							// Reflections quality
	char postproc;							// Post-processing quality
	char particles;							// Particles quality
	char miscfx;							// Miscellaneous FX quality

	float render_dist1;						// Render distance 1
	float render_dist2;						// Render distance 2

	bool subtitles;							// Subtitles
	char subt_kind;							// Subtitles options
};

struct AudioParams
{
	char vol_master;						// Master volume
	char vol_dialogue;						// Dialogue volume
	char vol_fx;							// Effect volume
	char vol_music;							// Music volume
};

struct GameParams
{
	bool debug;								// Debugging features
	bool cheats;							// sv_cheats 1
	char difficulty;						// Game difficulty
	bool camera_friction;					// Responsiveness of camera movement

	bool dbg_wireframe;						// Bullet debugging wireframe rendering
	int dbg_entityfollow;					// Entity followed by camera (for debugging)

	char lang_main;							// Main language
	char lang_subtitles;					// Subtitles language
	char lang_audio;						// Audio language
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