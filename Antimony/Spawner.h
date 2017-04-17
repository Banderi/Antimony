#pragma once

#include <string>
#include "Geometry.h"

///

typedef bool(*antSpawnCallback)(std::wstring id, float3 pos);

///

struct SpawnItem
{
	std::wstring id;
	antSpawnCallback spawn;
};