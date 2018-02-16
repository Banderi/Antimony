//#include <algorithm>
//#include <iostream>
#include <iterator>			// required for PathRemoveFileSpec()

#include "Warnings.h"
#include "FBX.h"
#include "Debug.h"
#include "Console.h"
#include "CConvertions.h"
#include "Path.h"

#define FBXSDK_SHARED

#if _WIN32 || _WIN64
	#if _WIN64
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

#ifdef ENVIRONMENT32
	#ifdef FBXSDK_SHARED
		#ifdef _DEBUG
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk.lib")
		#else
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk-static_d.lib")
		#else
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk-static.lib")
		#endif
	#endif
#elif defined(ENVIRONMENT64)
	#ifdef FBXSDK_SHARED
		#ifdef _DEBUG
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x64\\libfbxsdk_d.lib")
		#else
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x64\\libfbxsdk.lib")
		#endif
	#else
		#ifdef _DEBUG
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk-static_d.lib")
		#else
			#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk-static.lib")
		#endif
	#endif
#endif

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
float4 FbxToFloat4(FbxVector4 *fbx)
{
	return float4(fbx->mData[0], fbx->mData[1], fbx->mData[2], fbx->mData[3]);
}
Quaternion FbxToQuaternion(FbxQuaternion *fbx)
{
	return Quaternion(fbx->mData[0], fbx->mData[1], fbx->mData[2], fbx->mData[3]);
}
mat FbxToMat(FbxAMatrix *fbx)
{
	float m[16];
	int i = 0;
	for (int row = 0; row < 4; ++row)
	{
		for (int col = 0; col < 4; ++col)
		{
			m[i++] = (float)fbx->Get(row, col);
		}
	}
	return (mat)m;
}

bool LoadFBXFile(std::wstring file, FbxManager *fbxmngr, FbxImporter *fbximport, FbxScene *scene)
{
	bool result = false;

	std::wstring fullpath;
	fullpath = Antimony::filePath(L"\\Models\\", file);

	//fullpath = L".\\Models\\" + file;

	char *fullpathFBX = new char[MAX_PATH];
	FbxWCToUTF8(fullpath.c_str(), fullpathFBX);

	if (file.length() < 1)
	{
		Antimony::log(L"Could not open FBX scene from null path!\n", CSL_ERR_GENERIC);
		return false;
	}
	else
	{
		if (!fbximport->Initialize(fullpathFBX, -1, fbxmngr->GetIOSettings()))
		{
			Antimony::log(L"Call to FbxImporter::Initialize() failed! error: " + cc_wstr(fbximport->GetStatus().GetErrorString()) + L"\n", CSL_ERR_GENERIC);
			return false;
		}
		else
		{
			fbximport->Import(scene);

			// set scene name again cause FBX is stupid
			file = file.substr(0, file.size() - 4);
			char *fileFBX = new char[MAX_PATH];
			FbxWCToUTF8(file.c_str(), fileFBX);
			scene->SetName(fileFBX);

			int lFileMajor, lFileMinor, lFileRevision;
			fbximport->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

			std::wstring version = std::to_wstring(lFileMajor) + L"." + std::to_wstring(lFileMinor) + L"." + std::to_wstring(lFileRevision);

			Antimony::log(L"Loaded FBX scene from file '" + file + L"' (FBX v" + version + L")\n", CSL_COMMAND);
			return true;
		}
	}
}
std::vector<FbxMesh*> GetFBXMesh(FbxScene *scene)
{
	std::vector<FbxMesh*> meshes;

	FbxNode *pRoot = scene->GetRootNode();
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
VertexCompound GetVertexCompound(FbxMesh *mesh)
{
	VertexCompound compound;



	unsigned long pcount = mesh->GetPolygonCount();
	unsigned long vcount = mesh->GetPolygonVertexCount();

	int v_globalindex = 0;
	int v_finalindex = 0;

	bool has_normals = false;
	bool has_uvs = false;

	for (unsigned long n = 0; n < vcount; n++)							// fill vectors with dummy data
	{
		compound.position.push_back(float3(0, 0, 0));
		compound.normal.push_back(float3(0, 0, 0));
		compound.uv.push_back(float2(0, 0));
	}

	FbxGeometryElementNormal* nelem = mesh->GetElementNormal();
	//if (nelem)
	//{
	//	//numNormals = pMesh->GetPolygonCount() * 3;
	//	//normals = new float[numNormals * 3];
	//	int v = 0;
	//	for (int p = 0; pcount; p++)
	//	{
	//		for (int i = 0; i < 3; i++)
	//		{
	//			FbxVector4 normal = nelem->GetDirectArray().GetAt(v);
	//			compound.normal.at(v_globalindex) = FbxToFloat3(&normal);
	//			/*normals[vertexCounter * 3 + 0] = normal[0];
	//			normals[vertexCounter * 3 + 1] = normal[1];
	//			normals[vertexCounter * 3 + 2] = normal[2];*/
	//			//cout << "\n" << has_normals[vertexCounter * 3 + 0] << " " << has_normals[vertexCounter * 3 + 1] << " " << has_normals[vertexCounter * 3 + 2];
	//			v++;
	//		}
	//	}
	//}

	for (unsigned long p = 0; p < pcount; p++)								// for each polygon in mesh
	{
		unsigned long polyvcount = mesh->GetPolygonSize(p);
		for (unsigned long v = 0; v < polyvcount; v++)						// for each vertex in poly
		{


			v_globalindex = mesh->GetPolygonVertex(p, v);					// get vertex global index
			assert(v_globalindex != -1 && "Index is out of range");

			FbxVector4 vertex = mesh->GetControlPointAt(v_globalindex);

			/*FbxVector4 normal;
			has_normals = has_normals || pMesh->GetPolygonVertexNormal(p, v, normal);*/

			if (nelem)
			{
				has_normals = true;
				FbxVector4 normal = nelem->GetDirectArray().GetAt(v_globalindex);
				compound.normal.at(v_globalindex) = FbxToFloat3(&normal);
			}

			FbxVector2 uv;
			int uv_globalindex = -1;
			bool oneuvset = true;
			int uvsetcount = 1;
			if (!oneuvset)
				uvsetcount = mesh->GetElementUVCount();

			for (int uvset = 0; uvset < uvsetcount; uvset++)
			{
				FbxGeometryElementUV* geomElementUV = mesh->GetElementUV(uvset);

				if (geomElementUV)
				{
					FbxLayerElement::EMappingMode mapMode = geomElementUV->GetMappingMode();
					FbxLayerElement::EReferenceMode refMode = geomElementUV->GetReferenceMode();

					if (FbxGeometryElement::eByControlPoint == mapMode)
					{
						if (FbxGeometryElement::eDirect == refMode)
							uv_globalindex = v_globalindex;
						else if (FbxGeometryElement::eIndexToDirect == refMode)
							uv_globalindex = geomElementUV->GetIndexArray().GetAt(v_globalindex);
					}
					else if (FbxGeometryElement::eByPolygonVertex == mapMode)
					{
						if (FbxGeometryElement::eDirect == refMode || FbxGeometryElement::eIndexToDirect == refMode)
							uv_globalindex = mesh->GetTextureUVIndex(p, v);
					}


					if (uv_globalindex != -1)									// if we got a UV index
					{
						has_uvs = true;
						uv = geomElementUV->GetDirectArray().GetAt(uv_globalindex);
					}
				}
			}

			// ...

			compound.position.at(v_globalindex) = FbxToFloat3(&vertex);
			//compound.normal.at(v_globalindex) = FbxToFloat3(&normal);
			compound.uv.at(v_globalindex) = FbxToFloat2(&uv);

			if (v_finalindex < v_globalindex)
				v_finalindex = v_globalindex;

			/*if (v_globalindex == 0)
				auto asf = 325;
			if (compound.index.size() > 2996)
				auto asf = 2143;*/

			compound.index.push_back(v_globalindex);

			if ((compound.index.size()) % 3 == 0)
			{
				auto t = compound.index.at(compound.index.size() - 3);
				compound.index.at(compound.index.size() - 3) = v_globalindex;
				compound.index.at(compound.index.size() - 1) = t;
			}
		}
	}

	compound.position.erase(compound.position.begin() + v_finalindex + 1, compound.position.end());
	if (!has_normals)
		compound.normal.clear();
	else
		compound.normal.erase(compound.normal.begin() + v_finalindex + 1, compound.normal.end());
	if (!has_uvs)
		compound.uv.clear();
	else
		compound.uv.erase(compound.uv.begin() + v_finalindex + 1, compound.uv.end());

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
	//		for (unsigned long p = 0; p < pcount; p++)								// for each polygon in mesh
	//		{
	//			unsigned long vcount = pMesh->GetPolygonSize(p);
	//			for (unsigned long i = 0; i < vcount; i++)						// for each vertex in poly
	//			{
	//				int v_globalindex = pMesh->GetPolygonVertex(p, i);			// get vertex global index
	//				assert(v_globalindex != -1 && "Index is out of range");

	//				FbxVector4 vertex = pMesh->GetControlPointAt(v_globalindex);

	//				FbxVector4 normal;
	//				bool bResult = pMesh->GetPolygonVertexNormal(p, i, normal);

	//				FbxVector2 uv;
	//				int uvindex = lUseIndex ? uvelement->GetIndexArray().GetAt(v_globalindex) : v_globalindex;
	//				uv = uvelement->GetDirectArray().GetAt(uvindex);

	//				// ...

	//				compound.position.push_back(FbxToFloat3(&vertex));
	//				compound.normal.push_back(FbxToFloat3(&normal));
	//				compound.uv.push_back(FbxToFloat2(&uv));
	//				compound.index.push_back(v_globalindex);
	//			}
	//		}
	//	}
	//	else if (uvelement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	//	{
	//		int v_globalindex = 0;
	//		for (unsigned long p = 0; p < pcount; p++)								// for each polygon in mesh
	//		{
	//			unsigned long vcount = pMesh->GetPolygonSize(p);
	//			for (unsigned long i = 0; i < vcount; i++)						// for each vertex in poly
	//			{
	//				if (v_globalindex < vert_globalcount)
	//				{
	//					FbxVector4 vertex = pMesh->GetControlPointAt(v_globalindex);

	//					FbxVector4 normal;
	//					bool bResult = pMesh->GetPolygonVertexNormal(p, i, normal);

	//					FbxVector2 uv;
	//					int uvindex = lUseIndex ? uvelement->GetIndexArray().GetAt(v_globalindex) : v_globalindex;
	//					uv = uvelement->GetDirectArray().GetAt(uvindex);

	//					// ...

	//					compound.position.push_back(FbxToFloat3(&vertex));
	//					compound.normal.push_back(FbxToFloat3(&normal));
	//					compound.uv.push_back(FbxToFloat2(&uv));
	//					compound.index.push_back(v_globalindex);

	//					// ...

	//					v_globalindex++;
	//				}
	//			}
	//		}
	//	}
	//}
	return compound;
}

namespace Antimony
{
	FbxManager *FBXManager;
	FbxImporter *FBXImporter;
}