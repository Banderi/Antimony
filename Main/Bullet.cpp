#include "Bullet.h"
#include "Geometry.h"

///

btVector3 bt_origin = btVector3(0, 0, 0);

btDiscreteDynamicsWorld *btWorld;
std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> objectsCollisionPoints;
std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> objectsCollisions;

///

btTransform btObject::getbtTransform()
{
	btTransform t;
	ms->getWorldTransform(t);
	return t;
}
mat btObject::getMatTransform()
{
	btTransform t;
	ms->getWorldTransform(t);
	return btToMat(&t);
}
void btObject::setbtTransform(btTransform *m)
{
	rb->setWorldTransform(*m);
	ms->setWorldTransform(*m); // required for kinematic objects
}
void btObject::setMatTransform(mat *m)
{
	rb->setWorldTransform(MatTobt(m));
	ms->setWorldTransform(MatTobt(m)); // required for kinematic objects
}
btVector3 btObject::getbtPos()
{
	return getbtTransform().getOrigin();
}
float3 btObject::getFlat3Pos()
{
	return MatToFloat3(&getMatTransform());
}
void btObject::reset()
{
	rb->clearForces();
	rb->setLinearVelocity(btVector3(0, 0, 0));
	rb->setAngularVelocity(btVector3(0, 0, 0));
	this->setMatTransform(&m_initialtransform);
	rb->activate();
}
btVector3 btObject::updateKinematic(double delta)
{
	btTransform t;
	ms->getWorldTransform(t);

	btVector3 v = t.getOrigin() - m_oldKinematicTransform.getOrigin();

	m_oldKinematicTransform = t;

	rb->setLinearVelocity(v / delta);

	return v / delta;
}
btObject::btObject(LPCWSTR n, float m, btCollisionShape *cshape, btDefaultMotionState *mstate, btDiscreteDynamicsWorld *w)
{
	name = n;
	mass = m;
	cs = cshape;
	ms = mstate;
	m_world = w;

	in = new btVector3;
	cs->calculateLocalInertia(mass, *in);

	btRigidBody::btRigidBodyConstructionInfo rbci = btRigidBody::btRigidBodyConstructionInfo(mass, ms, cs, *in);
	rb = new btRigidBody(rbci);
	m_world->addRigidBody(rb);

	m_initialtransform = this->getMatTransform();
	physEntities.push_back(this);
}
btObject::btObject(LPCWSTR n, float m, btCollisionShape *cshape, btDefaultMotionState *mstate, btVector3 *inertia, btDiscreteDynamicsWorld *w)
{
	name = n;
	mass = m;
	cs = cshape;
	ms = mstate;
	in = inertia;
	m_world = w;

	btRigidBody::btRigidBodyConstructionInfo rbci = btRigidBody::btRigidBodyConstructionInfo(mass, ms, cs, *in);
	rb = new btRigidBody(rbci);
	m_world->addRigidBody(rb);

	m_initialtransform = this->getMatTransform();
	physEntities.push_back(this);
}
btObject::~btObject()
{
	delete cs;
	delete ms;
	delete rb;
	delete in;
}

std::vector<btObject*> physEntities;

void DXDebugDrawer::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}
void DXDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
{
	btVector3 p1 = from;
	btVector3 p2 = to;
	Draw3DLineThin(
		btToFloat3(&p1),
		btToFloat3(&p2),
		color(fromColor.getX(), fromColor.getY(), fromColor.getZ(), 1),
		color(toColor.getX(), toColor.getY(), toColor.getZ(), 1),
		&mat_identity);
}
void DXDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& col)
{
	/*D3DXVECTOR3 vPoints[2];
	vPoints[0].x = from.getX();
	vPoints[0].y = from.getY();
	vPoints[0].z = from.getZ();
	vPoints[1].x = to.getX();
	vPoints[1].y = to.getY();
	vPoints[1].z = to.getZ();
	d3ddev->SetFVF(D3DFVF_XYZ);
	d3ddev->SetVertexShader(NULL);
	d3ddev->SetPixelShader(NULL);
	D3DXMATRIX worldMatrix;
	D3DXMatrixIdentity(&worldMatrix);
	device->SetTransform(D3DTS_WORLD, &worldMatrix);
	device->DrawPrimitiveUP(D3DPT_LINELIST, 1, vPoints, sizeof(D3DXVECTOR3));*/

	btVector3 p1 = from;
	btVector3 p2 = to;
	color c = color(col.getX(), col.getY(), col.getZ(), 1);
	Draw3DLineThin(
		btToFloat3(&p1),
		btToFloat3(&p2),
		c,
		c,
		&mat_identity);
}
void DXDebugDrawer::drawSphere(const btVector3& p, btScalar radius, const btVector3& col)
{

}
void DXDebugDrawer::drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& col, btScalar alpha)
{

}
void DXDebugDrawer::draw3dText(const btVector3& location, const char *textString)
{

}
void DXDebugDrawer::drawContactPoint(const btVector3& pointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& col)
{

}
void DXDebugDrawer::reportErrorWarning(const char *warningString)
{

}
DXDebugDrawer::DXDebugDrawer()
{
	m_debugMode = 0;
}
DXDebugDrawer::~DXDebugDrawer()
{
	//
}

///

float3 btToFloat3(btVector3 *v)
{
	return float3(v->getX(), v->getY(), v->getZ());
}
float3 btToFloat3(const btVector3 *v)
{
	return float3(v->getX(), v->getY(), v->getZ());
}
mat btToMat(btTransform *t)
{
	btScalar r[16];
	t->getOpenGLMatrix(r);
	return (mat)r;
}
mat btToMat(const btTransform *t)
{
	btScalar r[16];
	t->getOpenGLMatrix(r);
	return (mat)r;
}
btVector3 Float3Tobt(float3 *f)
{
	return btVector3(f->x, f->y, f->z);
}
btVector3 Float3Tobt(const float3 *f)
{
	return btVector3(f->x, f->y, f->z);
}
btTransform MatTobt(mat *m)
{
	btTransform t;
	t.setFromOpenGLMatrix((float*)m);
	return t;
}

void TickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
	objectsCollisions.clear();
	objectsCollisionPoints.clear();
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto *objA = contactManifold->getBody0();
		auto *objB = contactManifold->getBody1();
		auto& manifoldsA = objectsCollisions[objA];
		auto& manifoldsB = objectsCollisions[objA];
		manifoldsA.push_back(contactManifold);
		manifoldsB.push_back(contactManifold);
		auto& collisionsA = objectsCollisionPoints[objA];
		auto& collisionsB = objectsCollisionPoints[objB];
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			collisionsA.push_back(&pt);
			collisionsB.push_back(&pt);
		}
	}
}