#ifndef FBX_H
#define FBX_H

#include <vector>
#include <Windows.h>
#include "../FBXSDK/include/fbxsdk.h"

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

#ifdef ENVIRONMENT32
#ifdef _DEBUG
#pragma comment (lib, "../FBXSDK/lib/x86/libfbxsdk-mtd.lib")
#else
#pragma comment (lib, "../FBXSDK/lib/x86/libfbxsdk-mt.lib")
#endif
#elif defined(ENVIRONMENT64)
#ifdef _DEBUG
#pragma comment (lib, "../FBXSDK/lib/x64/libfbxsdk-mtd.lib")
#else
#pragma comment (lib, "../FBXSDK/lib/x64/libfbxsdk-mt.lib")
#endif
#endif

struct MyVertex
{
	float pos[3];
};

extern FbxManager* g_pFbxSdkManager;

HRESULT LoadFBX(std::vector<MyVertex>* pOutVertexVector);

#endif