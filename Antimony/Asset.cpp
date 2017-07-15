#include "Asset.h"
#include "FBX.h"
#include "Antimony.h"

///

void Asset::LoadFBX(std::string file)
{
	FbxScene *scene = FbxScene::Create(antimony.FbxManager, "myScene");
	if (LoadFBXFile(file, antimony.FbxManager, antimony.FbxImporter, scene))
		m_geometry = GetVertexCompound(GetFBXMesh(scene).at(0));
}
void Asset::LoadTexture(std::string file)
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
void Asset::draw(mat *world, bool debug)
{
	if (m_geometry.position.size() < 1)
		return;

	mat s = XMMatrixScaling(-0.0016, 0.0016, 0.0016);
	mat r = XMMatrixRotationAxis(float3(0, 0, 1), MATH_PI / 2) * XMMatrixRotationAxis(float3(1, 0, 0), -MATH_PI / 2);
	*world = s * r * *world;

	DrawMesh(&m_geometry, world);

	if (debug)
	{
		auto c = WorldToScreen(XMVector3Transform(v3_origin, *world), &(mat_view * mat_proj), float2(antimony.display.width, antimony.display.height));
		Draw2DDot(float2(c.x, c.y), 4, COLOR_GREEN);

		auto vertices = m_geometry.position.size();
		for (unsigned int v = 0; v < vertices; v++)
		{
			auto p = WorldToScreen(XMVector3Transform(m_geometry.position.at(v), *world), &(mat_view * mat_proj), float2(antimony.display.width, antimony.display.height));
			Draw2DDot(float2(p.x, p.y), 2, COLOR_RED);

			auto cfade = 255 * v / vertices;
			auto color = RGBA2DWORD(cfade, 0, 0, 255);
			antimony.Consolas.render(std::to_wstring(v).c_str(), 12, antimony.display.width / 2 + p.x + 1, antimony.display.height / 2 + p.y + 1, color, NULL);
		}
	}
}