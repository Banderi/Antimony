#pragma once

#include <Windows.h>
#include <fbxsdk.h>
#include <vector>

//#if _WIN32 || _WIN64
//#if _WIN64
//#define ENVIRONMENT64
//#else
//#define ENVIRONMENT32
//#endif
//#endif
//
//#ifdef ENVIRONMENT32
//#ifdef _DEBUG
//#pragma comment (lib, "..\\FBXSDK\\lib\\x86\\libfbxsdk-mtd.lib")
//#else
//#pragma comment (lib, "..\\FBXSDK\\lib\\x86\\libfbxsdk-mt.lib")
//#endif
//#elif defined(ENVIRONMENT64)
//#ifdef _DEBUG
//#pragma comment (lib, "..\\FBXSDK\\lib\\x86\\libfbxsdk-mtd.lib")
//#else
//#pragma comment (lib, "..\\FBXSDK\\lib\\x86\\libfbxsdk-mt.lib")
//#endif
//#endif

///

extern FbxManager* g_pFbxSdkManager;

///

struct MyVertex
{
	float pos[3];
};

///

HRESULT LoadFBX(std::vector<MyVertex>* pOutVertexVector);
