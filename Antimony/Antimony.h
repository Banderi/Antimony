#pragma once

#include <fstream>		// required for std::wofstream								(Antimony::m_logfile)
#include <windows.h>	// required for Psapi.h
#include <Psapi.h>		// required for PROCESS_MEMORY_COUNTERS_EX					(Antimony::pmc)

#include "fbxsdk.h"

#include "Console.h"
//#include "Param.h"
#include "Input.h"
//#include "Bullet.h"
//#include "Geometry.h"
#include "FontRenderer.h"
#include "CpuUsage.h"
#include "Player.h"
#include "Camera.h"
#include "Spawner.h"

///

class Antimony
{
private:
	double m_delta;
	float m_worldSpeed;
	std::wofstream m_logFile;

	btDiscreteDynamicsWorld *m_btWorld;
	std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> m_objectsCollisionPoints;
	std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> m_objectsCollisions;
	std::vector<btObject*> m_physEntities;

	unsigned char m_subSystem;
	unsigned int m_gameState;

	wchar_t m_globalStr64[64];

	CpuUsage m_cpuUsage;

	DWORDLONG m_totalPhysMem, m_physMemAvail;
	ULONG_PTR m_physMemUsedByMe;
	MEMORYSTATUSEX m_memInfo;
	PROCESS_MEMORY_COUNTERS_EX m_pmc;

	Player m_player;
	Camera m_camera;

	RAWINPUTDEVICE m_rid[4];

	MouseController m_mouse;
	KeysController m_keys;
	XInputController m_controller[4];

	std::vector<SpawnItem> m_spawnables;

	IFW1Factory *m_FW1Factory;

public:
	FontWrapper Arial, Consolas;

	FbxManager *FbxManager;
	FbxImporter *FbxImporter;

	WindowParams window_main;
	DisplayParams display;
	AudioParams audio;
	GameParams game;
	ControlParams controls;

	Console devConsole;

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

	HRESULT initDebugMonitor();
	void unacquireDebugMonitor();
	void monitorLog();
	HRESULT log(std::wstring string, unsigned int col, bool timestamp = true);
	HRESULT logVolatile(std::wstring string);
	void logError(HRESULT hr);

	btDiscreteDynamicsWorld* getBtWorld();
	void tickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	static void staticCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	void addPhysEntity(btObject *obj);
	std::vector<btObject*>* getEntities();
	void resetPhysics();

	double getTick();
	double getDelta();

	void setGameState(unsigned int state);
	unsigned int getGameState();
	bool ifGameState(unsigned int state);

	void setSubSystem(unsigned char subs);
	unsigned char getSubSystem();
	bool ifSubSystem(unsigned char subs);

	Player* getPlayer();
	Camera* getCamera();

	bool addSpawnable(std::wstring id, antSpawnCallback call);
	bool spawn(std::wstring id, unsigned int qu, float3 coord);
	static bool standardSpawn(std::wstring id, float3 pos);

	Antimony()
	{
		m_delta = 0;
		m_worldSpeed = 1;
	}
};

extern Antimony antimony;