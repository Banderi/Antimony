#pragma once

#include <string>
#include <vector>

#include "fbxsdk.h"

#include "Geometry.h"

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT32
#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x86\\libfbxsdk.lib")
#elif defined(ENVIRONMENT64)
#pragma comment (lib, "..\\ext\\FBXSDK\\lib\\x64\\libfbxsdk.lib")
#endif

///

float2 FbxToFloat2(FbxVector2 *fbx);
float2 FbxToFloat2(FbxVector4 *fbx);
float3 FbxToFloat3(FbxVector4 *fbx);

bool LoadFBXFile(std::string lFilePath, FbxManager *fbxmngr, FbxImporter *fbximport, FbxScene *lScene);

std::vector<FbxMesh*> GetFBXMesh(FbxScene *lScene);
VertexCompound GetVertexCompound(FbxMesh *pMesh);
VertexCompound GetVertexCompound(std::vector<FbxMesh*> pMeshes);

void DrawMesh(VertexCompound *mesh, mat *world = &mat_world, color diffuse = COLOR_WHITE, ID3D11Device *dv = dev, ID3D11DeviceContext *devc = devcon, SHADER* sh = sh_current, ID3D11Buffer *ib = indexbuffer, ID3D11Buffer *cb = constantbuffer);