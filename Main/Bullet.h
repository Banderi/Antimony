#pragma once

#include <vector>
#include <map>

#include "Warnings.h"
#include "DirectX.h"

#include "btBulletDynamicsCommon.h"
//#include "BulletDynamics\MLCPSolvers\btMLCPSolver.h"
//#include "BulletDynamics\MLCPSolvers\btDantzigSolver.h"
#include "BulletDynamics\Character\btKinematicCharacterController.h"
#include "BulletCollision\CollisionDispatch\btGhostObject.h"
#include "LinearMath\btIDebugDraw.h"

#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "LinearMath.lib")

///

#define WORLD_SCALE 1.1f

///

extern btVector3 bt_origin;

extern btDiscreteDynamicsWorld *btWorld;
extern std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> objectsCollisionPoints;
extern std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> objectsCollisions;

///

__declspec(align(16)) class btObject
{
protected:
	btDiscreteDynamicsWorld *m_world;
	mat m_initialtransform;
	btTransform m_oldKinematicTransform;

public:
	LPCWSTR name;
	float mass;
	btCollisionShape *cs;
	btDefaultMotionState *ms;
	btVector3 *in;
	btRigidBody *rb;

	btTransform getbtTransform();
	mat getMatTransform();
	void setbtTransform(btTransform *m);
	void setMatTransform(mat *m);
	btVector3 getbtPos();
	float3 getFlat3Pos();
	void reset();
	btVector3 updateKinematic(double delta);

	btObject(LPCWSTR n, float m, btCollisionShape *cshape, btDefaultMotionState *mstate, btDiscreteDynamicsWorld *w = btWorld);
	btObject(LPCWSTR n, float m, btCollisionShape *cshape, btDefaultMotionState *mstate, btVector3 *inertia, btDiscreteDynamicsWorld *w = btWorld);
	~btObject();

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}
};

extern std::vector<btObject*> physEntities;

class DXDebugDrawer : public btIDebugDraw
{
	int m_debugMode;

public:

	DXDebugDrawer();
	virtual ~DXDebugDrawer();

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& col);

	virtual void   drawSphere(const btVector3& p, btScalar radius, const btVector3& col);

	virtual void   drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& col, btScalar alpha);

	virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& col);

	virtual void   reportErrorWarning(const char* warningString);

	virtual void   draw3dText(const btVector3& location, const char* textString);

	virtual void   setDebugMode(int debugMode);

	virtual int    getDebugMode() const { return m_debugMode; }

};

///

float3 btToFloat3(btVector3 *v);
float3 btToFloat3(const btVector3 *v);
mat btToMat(btTransform *t);
mat btToMat(const btTransform *t);

btVector3 Float3Tobt(float3 *f);
btVector3 Float3Tobt(const float3 *f);
btTransform MatTobt(mat *m);

void TickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);