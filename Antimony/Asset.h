#pragma once

#include <string>

#include "Geometry.h"
#include "Texture.h"

///

class Asset
{
private:
	VertexCompound m_geometry;
	TextureBundle m_textures;

public:
	void LoadFBX(std::string file);
	void LoadTexture(std::string file);

	VertexCompound getGeometry();
	TextureBundle getTextures();

	void draw(mat *world = &mat_world, bool debug = false);
};