#pragma once

#include <fstream>		// required for std::wofstream								(Antimony::m_logfile)
#include <windows.h>	// required for Psapi.h
#include <Psapi.h>		// required for PROCESS_MEMORY_COUNTERS_EX					(Antimony::pmc)

#include <FW1FontWrapper.h>

#include "Param.h"
#include "Input.h"
#include "Bullet.h"
#include "Geometry.h"
#include "CpuUsage.h"

#pragma comment (lib, "..\\ext\\FW1FontWrapper\\lib\\x86\\FW1FontWrapper.lib")

///

class Antimony
{
private:
	double m_delta;
	float m_worldSpeed;
	std::wofstream m_logfile;

	btDiscreteDynamicsWorld *m_btWorld;
	std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> m_objectsCollisionPoints;
	std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> m_objectsCollisions;
	std::vector<btObject*> m_physEntities;

	unsigned char m_subSystem;
	unsigned int m_gameState;

	wchar_t m_globalStr64[64];

	IFW1Factory *m_fw1Factory;
	IFW1FontWrapper *m_fw1Arial, *m_fw1Courier;

	CpuUsage m_cpuUsage;

	DWORDLONG m_totalPhysMem, m_physMemAvail;
	ULONG_PTR m_physMemUsedByMe;
	MEMORYSTATUSEX m_memInfo;
	PROCESS_MEMORY_COUNTERS_EX m_pmc;

public:
	WindowParams window_main;
	DisplayParams display;
	AudioParams audio;
	GameParams game;
	ControlParams controls;

	int startUp(HINSTANCE hInstance, int nCmdShow);
	void readConfig();
	HRESULT enumHardware();
	HRESULT initD3D(HWND hWnd);
	HRESULT initControls();
	HRESULT initShaders();
	HRESULT initFonts();
	HRESULT initGraphics();
	HRESULT initPhysics();
	HRESULT loadStartingFiles();

	void createMainWindow(HINSTANCE hInstance);

	void cleanUp();
	void cleanD3D();
	void releaseFiles();

	void step();
	void endStep();
	HRESULT prepareFrame();
	HRESULT presentFrame();
	void updatePlayerControls(KeysController *khandle, XInputController *xhandle, double delta);
	void updateCameraControls(MouseController *mhandle, KeysController *khandle, XInputController *xhandle, double delta);
	void updateAI(double delta);
	void updatePhysics(double delta);
	void updateWorld(double delta);
	void updateGameState();

	void render_Debug();
	void render_DebugKeyboard(float2 pos);
	void render_DebugMouse(float2 pos);
	void render_DebugController(float2 pos, unsigned char c);
	void render_DebugFPS(float2 pos);

	HRESULT registerRID();
	HRESULT handleInput(MSG msg);

	void initializeDebugConsole();
	void shutdownDebugConsole();
	HRESULT writeToConsole(std::wstring string, bool t = true, bool logf = true);
	void consoleLog();
	void logError(HRESULT hr);
	bool handleErr(HRESULT *hOut, DWORD facing, HRESULT hr, const wchar_t* opt = L"");

	bool compileShader(HRESULT *hr, std::wstring shader, SHADER *sh);

	btDiscreteDynamicsWorld* getBtWorld();
	void tickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	static void staticCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	void addPhysEntity(btObject *obj);
	UINT getEntityCount();
	void resetPhysics();

	void setGameState(unsigned int state);
	unsigned int getGameState();
	bool ifGameState(unsigned int state);

	void setSubSystem(unsigned char subs);
	unsigned char getSubSystem();
	bool ifSubSystem(unsigned char subs);

	Antimony()
	{
		m_delta = 0;
		m_worldSpeed = 1;
	}
};

extern Antimony antimony;