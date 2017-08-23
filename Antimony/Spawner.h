#pragma once

#include <string>

#include "Warnings.h"
#include "Geometry.h"

///

typedef bool(*antSpawnCallback)(std::wstring id, float3 pos);

///

struct SpawnItem
{
	std::wstring id;
	antSpawnCallback spawn;
};

namespace Antimony
{
	extern std::vector<SpawnItem> spawnables;

	bool addSpawnable(std::wstring id, antSpawnCallback call);
	bool spawn(std::wstring id, unsigned int qu, float3 coord);
	bool standardSpawn(std::wstring id, float3 pos);
}