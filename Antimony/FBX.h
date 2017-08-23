#pragma once

#include <string>
#include <vector>

#define FBXSDK_SHARED
#include "fbxsdk.h"				// from /ext/FBXSDK/include/

#include "Geometry.h"

///

float2 FbxToFloat2(FbxVector2 *fbx);
float2 FbxToFloat2(FbxVector4 *fbx);
float3 FbxToFloat3(FbxVector4 *fbx);
Quaternion FbxToQuaternion(FbxQuaternion *fbx);
mat FbxToMat(FbxAMatrix *fbx);

bool LoadFBXFile(std::wstring file, FbxManager *fbxmngr, FbxImporter *fbximport, FbxScene *scene);

std::vector<FbxMesh*> GetFBXMesh(FbxScene *scene);
VertexCompound GetVertexCompound(FbxMesh *mesh);

namespace Antimony
{
	extern FbxManager *FBXManager;
	extern FbxImporter *FBXImporter;
}