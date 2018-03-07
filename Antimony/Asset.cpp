#include "Warnings.h"
#include "Asset.h"
#include "FBX.h"
#include "Param.h"
#include "FontRenderer.h"

///

void Asset::loadFBX(std::wstring file, float3 s)
{
	m_scale = s;

	char *fileFBX = new char[MAX_PATH];
	FbxWCToUTF8(file.c_str(), fileFBX);

	FbxScene *scene = FbxScene::Create(Antimony::FBXManager, fileFBX);
	if (LoadFBXFile(file, Antimony::FBXManager, Antimony::FBXImporter, scene))
	{
		m_geometry = GetVertexCompound(GetFBXMesh(scene).at(0));	// get mesh data from first (root node) mesh in the scene
		m_animcontroller.initialize(scene);							// initialize animation controller

		auto as = 325;
	}
}
void Asset::loadTexture(std::wstring file)
{

}
VertexCompound Asset::getGeometry()
{
	return m_geometry;
}
TextureBundle Asset::getTextures()
{
	return m_textures;
}
void Asset::draw(mat *mat_world, bool debug)
{
	if (m_geometry.position.size() < 1)
		return;

	mat s = MScalVector(m_scale);
	mat r = MRotAxis(float3(1, 0, 0), -MATH_PI / 2);
	mat w = s * r * *mat_world;

	Antimony::DrawMesh(&m_geometry, &w);

	if (debug)
	{
		drawSkeleton(mat_world, false);

		/*auto sh_temp = sh_current;
		Antimony::setShader(SHADERS_PLAIN);

		auto c = WorldToScreen(V3Transform(v3_origin, *world), &(mat_view * mat_proj), float2(Antimony::display.width, Antimony::display.height));
		Draw2DDot(float2(c.x, c.y), 4, COLOR_GREEN);

		auto vertices = m_geometry.position.size();
		for (unsigned int v = 0; v < vertices; v++)
		{
			auto p = WorldToScreen(XMVector3Transform(m_geometry.position.at(v), *world), &(mat_view * mat_proj), float2(Antimony::display.width, Antimony::display.height));
			if (p.z > 0)
			{
				Draw2DDot(float2(p.x, p.y), 2, COLOR_RED);

				auto cfade = 255 * v / vertices;
				auto color = RGBA2DWORD(cfade, 0, 0, 255);
				Antimony::Consolas.render(std::to_wstring(v).c_str(), 12, Antimony::display.width / 2 + p.x + 1, Antimony::display.height / 2 + p.y + 1, color, NULL);
			}
		}
		Antimony::setShader(sh_temp);*/
	}
}
void Asset::drawSkeleton(mat *mat_world, bool names)
{
	auto sh_temp = Antimony::sh_current;
	Antimony::setShader(SHADERS_PLAIN);

	if (!m_animcontroller.skeleton.valid)
		return;

	for (UINT i = 0; i < m_animcontroller.skeleton.joints_sequential.size(); i++)
	{
		Joint *joint = m_animcontroller.skeleton.joints_sequential.at(i);
		mat mat_parent = joint->parent ? joint->parent->transform : mat_identity;
		mat mat_bone = joint->transform;
		/*mat mat_parent = joint->parent ? *m_animcontroller.skeleton.transform_sequential.at(joint->parent->index) : mat_identity;
		mat mat_bone = *m_animcontroller.skeleton.transform_sequential.at(i);*/

		mat s = MScalVector(m_scale * 0.01);									// asset's scale
		mat w = s * *mat_world;													// final world transform

		auto p_parent = WorldToScreen(V3Transform(v3_origin, mat_parent * w));
		auto p_origin = WorldToScreen(V3Transform(v3_origin, mat_bone * w));
		auto p_offset = WorldToScreen(V3Transform(v3_origin, MTranslation(0, 0.1, 0) * mat_bone * w));

		if (!joint->parent)
		{
			auto p = WorldToScreen(V3Transform(v3_origin, *mat_world));
			if (p.z > 0)
				Antimony::Draw2DDot(float2(p.x, p.y), 4, COLOR_BLUE);									// blue dot for origin
		}
		else
		{
			if (joint->children.size() == 0)
			{
				Antimony::Draw2DDot(float2(p_origin.x, p_origin.y), 4, COLOR_GREEN);					// green dot for dangling bone
			}
			else
			{
				Antimony::Draw2DDot(float2(p_origin.x, p_origin.y), 4, COLOR_BLUE);					// blue dot for joint between bones
				if (p_offset.z > 0 && names)
					Antimony::Consolas.render(joint->name.c_str(), 12, Antimony::display.right + p_offset.x + 1, Antimony::display.bottom + p_offset.y + 1, RGBA2DWORD(128, 128, 255, 255), NULL);
			}

			Antimony::Draw2DLineThin((float2)p_parent, (float2)p_origin, COLOR_BLUE, COLOR_BLUE);		// bone (blue line)
		}
	}

	Antimony::setShader(sh_temp);
}
void Asset::update(double delta)
{
	m_animcontroller.update(delta);
}
AnimationController* Asset::animController()
{
	return &m_animcontroller;
}