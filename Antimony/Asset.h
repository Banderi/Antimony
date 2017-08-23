#pragma once

#include <string>

#include "Geometry.h"
#include "Texture.h"
#include "Animation.h"

///

class Asset
{
private:
	VertexCompound m_geometry;
	TextureBundle m_textures;
	AnimationController m_animcontroller;
	float3 m_scale;

public:
	void loadFBX(std::wstring file, float3 s);
	void loadTexture(std::wstring file);

	VertexCompound getGeometry();
	TextureBundle getTextures();

	void draw(mat *mat_world = &mat_world, bool debug = false);
	void drawSkeleton(mat *mat_world, mat *mat_parent, Joint *node, bool root);

	void update(double delta);

	AnimationController* animController();
};