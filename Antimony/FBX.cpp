#include "FBX.h"
#include "Antimony.h"
#include "CConvertions.h"

///

float2 FbxToFloat2(FbxVector2 *fbx)
{
	return float2(fbx->mData[0], fbx->mData[1]);
}
float2 FbxToFloat2(FbxVector4 *fbx)
{
	return float2(fbx->mData[0], fbx->mData[1]);
}
float3 FbxToFloat3(FbxVector4 *fbx)
{
	return float3(fbx->mData[0], fbx->mData[1], fbx->mData[2]);
}

bool LoadFBXFile(std::string file, FbxManager *fbxmngr, FbxImporter *fbximport, FbxScene *lScene)
{
	bool result = false;

	std::string fullpath = ".\\Models\\" + file;

	if (file.length() < 1)
	{
		antimony.log(L"Could not open FBX scene from null path!\n", CSL_ERR_GENERIC);
		return false;
	}
	else
	{
		if (!fbximport->Initialize(fullpath.c_str(), -1, fbxmngr->GetIOSettings()))
		{
			antimony.log(L"Call to FbxImporter::Initialize() failed! error: " + cc_wstr(fbximport->GetStatus().GetErrorString()) + L"\n", CSL_ERR_GENERIC);
			return false;
		}
		else
		{
			fbximport->Import(lScene);

			int lFileMajor, lFileMinor, lFileRevision;
			fbximport->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

			std::wstring version = std::to_wstring(lFileMajor) + L"." + std::to_wstring(lFileMinor) + L"." + std::to_wstring(lFileRevision);

			antimony.log(L"Loaded FBX scene from file '" + cc_wstr(file) + L"' (FBX v" + version + L")\n", CSL_COMMAND);
			return true;
		}
	}
}
std::vector<FbxMesh*> GetFBXMesh(FbxScene *lScene)
{
	FbxNode *pRoot = lScene->GetRootNode();

	std::vector<FbxMesh*> meshes;

	if (pRoot)
	{
		for (int n = 0; n < pRoot->GetChildCount(); n++)							// for each child node after the root node
		{
			FbxNode *pNode = pRoot->GetChild(n);
			if (pNode->GetNodeAttribute())
			{
				if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					FbxMesh *pMesh = (FbxMesh*)pNode->GetNodeAttribute();
					meshes.push_back(pMesh);
				}
			}
		}
	}
	return meshes;
}
VertexCompound GetVertexCompound(FbxMesh *pMesh)
{
	VertexCompound compound;



	unsigned long polies = pMesh->GetPolygonCount();
	unsigned long vertices = pMesh->GetPolygonVertexCount();

	int vert_globalindex = 0;

	bool normals = false;
	bool uvs = false;

	for (unsigned long n = 0; n < vertices; n++)							// fill vectors with dummy data
	{
		compound.position.push_back(float3(0, 0, 0));
		compound.normal.push_back(float3(0, 0, 0));
		compound.uv.push_back(float2(0, 0));
	}

	for (unsigned long p = 0; p < polies; p++)								// for each polygon in mesh
	{
		unsigned long polyvertices = pMesh->GetPolygonSize(p);
		for (unsigned long v = 0; v < polyvertices; v++)					// for each vertex in poly
		{
			vert_globalindex = pMesh->GetPolygonVertex(p, v);				// get vertex global index
			assert(vert_globalindex != -1 && "Index is out of range");

			FbxVector4 vertex = pMesh->GetControlPointAt(vert_globalindex);

			FbxVector4 normal;
			normals = normals || pMesh->GetPolygonVertexNormal(p, v, normal);

			FbxVector2 uv;
			int uv_globalindex = -1;
			bool oneuvset = true;
			int uvsetcount = 1;
			if (!oneuvset)
				uvsetcount = pMesh->GetElementUVCount();
			for (int uvset = 0; uvset < uvsetcount; uvset++)
			{
				FbxGeometryElementUV* geomElementUV = pMesh->GetElementUV(uvset);

				FbxLayerElement::EMappingMode mapMode = geomElementUV->GetMappingMode();
				FbxLayerElement::EReferenceMode refMode = geomElementUV->GetReferenceMode();

				if (FbxGeometryElement::eByControlPoint == mapMode)
				{
					if (FbxGeometryElement::eDirect == refMode)
						uv_globalindex = vert_globalindex;
					else if (FbxGeometryElement::eIndexToDirect == refMode)
						uv_globalindex = geomElementUV->GetIndexArray().GetAt(vert_globalindex);
				}
				else if (FbxGeometryElement::eByPolygonVertex == mapMode)
				{
					if (FbxGeometryElement::eDirect == refMode || FbxGeometryElement::eIndexToDirect == refMode)
						uv_globalindex = pMesh->GetTextureUVIndex(p, v);
				}


				if (uv_globalindex != -1)									// if we got a UV index
				{
					uvs = true;
					uv = geomElementUV->GetDirectArray().GetAt(uv_globalindex);
				}
			}

			// ...

			compound.position.at(vert_globalindex) = FbxToFloat3(&vertex);
			compound.normal.at(vert_globalindex) = FbxToFloat3(&normal);
			compound.uv.at(vert_globalindex) = FbxToFloat2(&uv);

			compound.index.push_back(vert_globalindex);
		}
	}

	compound.position.erase(compound.position.begin() + vert_globalindex + 1, compound.position.end());
	if (!normals)
		compound.normal.clear();
	else
		compound.normal.erase(compound.normal.begin() + vert_globalindex + 1, compound.normal.end());
	if (!uvs)
		compound.uv.clear();
	else
		compound.uv.erase(compound.uv.begin() + vert_globalindex + 1, compound.uv.end());

	assert((compound.index.size() % 3) == 0);
	for (auto it = compound.index.begin(); it != compound.index.end(); it += 3)
	{
		std::swap(*it, *(it + 2));
	}
	for (auto it = compound.uv.begin(); it != compound.uv.end(); ++it)
	{
		it->x = (1.f - it->x);
	}

	//FbxStringList uvsets;
	//pMesh->GetUVSetNames(uvsets);

	//bool oneuvset = true;

	//int uvsetcount = 1;
	//if (!oneuvset)
	//	uvsetcount = uvsets.GetCount();
	//for (int u = 0; u < uvsetcount; u++)											// for each UV set
	//{
	//	const char *uvsetname = uvsets.GetStringAt(u);								// get set name at index
	//	const FbxGeometryElementUV *uvelement = pMesh->GetElementUV(uvsetname);		// get set object (uvelement) from name

	//	if (!uvelement)
	//		continue;

	//	// only support mapping mode eByPolygonVertex and eByControlPoint
	//	if (uvelement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex &&
	//		uvelement->GetMappingMode() != FbxGeometryElement::eByControlPoint)
	//		return compound;

	//	// index array, where holds the index referenced to the uv data
	//	const bool lUseIndex = uvelement->GetReferenceMode() != FbxGeometryElement::eDirect;
	//	const int vert_globalcount = (lUseIndex) ? uvelement->GetIndexArray().GetCount() : 0;

	//	if (uvelement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	//	{
	//		for (unsigned long p = 0; p < polies; p++)								// for each polygon in mesh
	//		{
	//			unsigned long vertices = pMesh->GetPolygonSize(p);
	//			for (unsigned long i = 0; i < vertices; i++)						// for each vertex in poly
	//			{
	//				int vert_globalindex = pMesh->GetPolygonVertex(p, i);			// get vertex global index
	//				assert(vert_globalindex != -1 && "Index is out of range");

	//				FbxVector4 vertex = pMesh->GetControlPointAt(vert_globalindex);

	//				FbxVector4 normal;
	//				bool bResult = pMesh->GetPolygonVertexNormal(p, i, normal);

	//				FbxVector2 uv;
	//				int uvindex = lUseIndex ? uvelement->GetIndexArray().GetAt(vert_globalindex) : vert_globalindex;
	//				uv = uvelement->GetDirectArray().GetAt(uvindex);

	//				// ...

	//				compound.position.push_back(FbxToFloat3(&vertex));
	//				compound.normal.push_back(FbxToFloat3(&normal));
	//				compound.uv.push_back(FbxToFloat2(&uv));
	//				compound.index.push_back(vert_globalindex);
	//			}
	//		}
	//	}
	//	else if (uvelement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	//	{
	//		int vert_globalindex = 0;
	//		for (unsigned long p = 0; p < polies; p++)								// for each polygon in mesh
	//		{
	//			unsigned long vertices = pMesh->GetPolygonSize(p);
	//			for (unsigned long i = 0; i < vertices; i++)						// for each vertex in poly
	//			{
	//				if (vert_globalindex < vert_globalcount)
	//				{
	//					FbxVector4 vertex = pMesh->GetControlPointAt(vert_globalindex);

	//					FbxVector4 normal;
	//					bool bResult = pMesh->GetPolygonVertexNormal(p, i, normal);

	//					FbxVector2 uv;
	//					int uvindex = lUseIndex ? uvelement->GetIndexArray().GetAt(vert_globalindex) : vert_globalindex;
	//					uv = uvelement->GetDirectArray().GetAt(uvindex);

	//					// ...

	//					compound.position.push_back(FbxToFloat3(&vertex));
	//					compound.normal.push_back(FbxToFloat3(&normal));
	//					compound.uv.push_back(FbxToFloat2(&uv));
	//					compound.index.push_back(vert_globalindex);

	//					// ...

	//					vert_globalindex++;
	//				}
	//			}
	//		}
	//	}
	//}
	return compound;
}

void DrawMesh(VertexCompound *mesh, mat *world, color diffuse, ID3D11Device *dv, ID3D11DeviceContext *devc, SHADER* sh, ID3D11Buffer *ib, ID3D11Buffer *cb)
{
	UINT vcount = mesh->position.size();
	UINT icount = mesh->index.size();

	VERTEX_BASIC *vertices = mesh->dumpBASIC();
	UINT *indices = mesh->dumpIndices();

	auto vsize = sizeof(VERTEX_BASIC) * vcount;
	auto isize = sizeof(UINT) * icount;

	/*vsize = sizeof(v2);
	isize = sizeof(i2);*/

	//auto test = vertices[24];

	FillBuffer(dv, devc, &sh->vb, vertices, vsize);
	FillBuffer(dv, devc, &ib, indices, isize);

	setView(world, &mat_view, &mat_proj, diffuse, dv, devc, cb);
	devc->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devc->DrawIndexed(icount, 0, 0);
	return;
}