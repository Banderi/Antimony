#pragma once

#include "Geometry.h"

///

namespace Antimony
{
	void render_Debug();
	void render_DebugKeyboard(float2 pos);
	void render_DebugMouse(float2 pos);
	void render_DebugController(float2 pos, unsigned char c);
	void render_DebugFPS(float2 pos);
}