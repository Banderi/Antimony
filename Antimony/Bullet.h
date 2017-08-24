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

///

#define BTOBJECT_NULL				0x000
#define BTOBJECT_INFINITEGROUND		0x001
#define BTOBJECT_STATICWORLD		0x002
#define BTOBJECT_KINEMATICWORLD		0x003
#define BTOBJECT_DYNAMIC			0x004
#define BTOBJECT_SOFT				0x005
#define BTOBJECT_SHATTER			0x006
#define BTOBJECT_RAGDOLL			0x007
#define BTOBJECT_CAMERA				0x008

#define BTOBJECT_NPC				0x010
#define BTOBJECT_VEHICLE			0x011
#define BTOBJECT_PLAYER				0x014

#define BTSOLID_INFPLANE			0x00
#define BTSOLID_BOX					0x01
#define BTSOLID_SPHERE				0x02
#define BTSOLID_CYLINDER			0x03
#define BTSOLID_CAPSULE				0x04

#define WORLD_SCALE 1.0f

///

extern btVector3 bt_origin;

///

__declspec(align(16)) class btObject
{
protected:
	btDiscreteDynamicsWorld *m_world;

	btCollisionShape *m_collisionShape;
	btDefaultMotionState *m_motionState;
	btVector3 *m_inertia;
	btRigidBody *m_rigidBody;

	btTransform m_initialtransform;
	btTransform m_oldKinematicTransform;

	unsigned int m_kind;
	unsigned int m_primitive;
	float m_mass;
	float3 m_size;

	btCollisionShape* getShapeFromPrimitive(unsigned int p, float3 s);

public:
	btRigidBody* getRigidBody();
	int getKind();
	int getPrimitive();
	float getMass();
	float3 getPrimitiveSize();
	btVector3 getbtPos();
	float3 getFloat3Pos();
	btTransform getbtTransform();
	mat getMatTransform();
	void setbtTransform(btTransform *m);
	void setMatTransform(mat *m);

	btVector3 updateKinematic(double delta);
	void reset();
	void initObject();

	btObject(unsigned int k, unsigned int p, float m, float3 z, btDefaultMotionState *s, btDiscreteDynamicsWorld *w);
	btObject(unsigned int k, unsigned int p, float m, float3 z, btDefaultMotionState *s, btVector3 *i, btDiscreteDynamicsWorld *w);
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

class DXDebugDrawer : public btIDebugDraw
{
	int m_debugMode;

public:

	DXDebugDrawer();
	virtual ~DXDebugDrawer();

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& col);
	virtual void drawSphere(const btVector3& p, btScalar radius, const btVector3& col);
	virtual void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& col, btScalar alpha);
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& col);
	virtual void reportErrorWarning(const char* warningString);
	virtual void draw3dText(const btVector3& location, const char* textString);
	virtual void setDebugMode(int debugMode);
	virtual int getDebugMode() const
	{
		return m_debugMode;
	}
};

///

float3 btToFloat3(btVector3 *v);
float3 btToFloat3(const btVector3 *v);
mat btToMat(btTransform *t);
mat btToMat(const btTransform *t);

btVector3 Float3Tobt(float3 *f);
btVector3 Float3Tobt(const float3 *f);
btTransform MatTobt(mat *m);

btVector3 btQuaternionToEuler(const btQuaternion &TQuat);

namespace Antimony
{
	extern btDiscreteDynamicsWorld *btWorld;
	extern std::vector<btObject*> physEntities;

	extern std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> m_objectsCollisionPoints;
	extern std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> m_objectsCollisions;

	void tickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	void staticCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep);
	void addPhysEntity(btObject *obj);
	void resetPhysics();
	/*btDiscreteDynamicsWorld* getBtWorld();
	std::vector<btObject*>* getEntities();*/
}