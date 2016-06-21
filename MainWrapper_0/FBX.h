#ifndef FBX_H
#define FBX_H

#include <vector>
#include <Windows.h>
#include <fbxsdk.h>

#pragma comment (lib, "libfbxsdk.lib")

struct MyVertex
{
	float pos[3];
};

extern FbxManager* g_pFbxSdkManager;

HRESULT LoadFBX(std::vector<MyVertex>* pOutVertexVector);

#endif