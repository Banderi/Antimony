#include "Bullet.h"
#include "Geometry.h"

#pragma comment(lib, "BulletDynamics.lib")
#pragma comment(lib, "BulletCollision.lib")
#pragma comment(lib, "LinearMath.lib")

///

btVector3 bt_origin = btVector3(0, 0, 0);

///

btCollisionShape* btObject::getShapeFromPrimitive(unsigned int p, float3 s)
{
	btCollisionShape *cs;
	switch (p)
	{
		case BTSOLID_INFPLANE:
		{
			cs = new btStaticPlaneShape(btVector3(s.x, s.y, s.z), 1);
			break;
		}
		case BTSOLID_BOX:
		{
			cs = new btBoxShape(WORLD_SCALE * Float3Tobt(&s));
			break;
		}
		case BTSOLID_SPHERE:
		{
			cs = new btSphereShape(WORLD_SCALE * s.x);
			break;
		}
		case BTSOLID_CYLINDER:
		{
			cs = new btCylinderShape(WORLD_SCALE * Float3Tobt(&s));
			break;
		}
		case BTSOLID_CAPSULE:
		{
			cs = new btCapsuleShape(WORLD_SCALE * s.x, WORLD_SCALE * s.y);
			break;
		}
		default:
		{
			cs = nullptr;
			break;
		}

	}
	return cs;
}
btRigidBody* btObject::getRigidBody()
{
	return m_rigidBody;
}
int btObject::getKind()
{
	return m_kind;
}
int btObject::getPrimitive()
{
	return m_primitive;
}
float btObject::getMass()
{
	return m_mass;
}
float3 btObject::getPrimitiveSize()
{
	return m_size;
}
btVector3 btObject::getbtPos()
{
	return getbtTransform().getOrigin();
}
float3 btObject::getFloat3Pos()
{
	return MatToFloat3(&getMatTransform());
}
btTransform btObject::getbtTransform()
{
	btTransform t;
	m_motionState->getWorldTransform(t);
	return t;
}
mat btObject::getMatTransform()
{
	btTransform t;
	m_motionState->getWorldTransform(t);
	return btToMat(&t);
}
void btObject::setbtTransform(btTransform *m)
{
	m_rigidBody->setWorldTransform(*m);
	m_motionState->setWorldTransform(*m); // required for kinematic objects
}
void btObject::setMatTransform(mat *m)
{
	m_rigidBody->setWorldTransform(MatTobt(m));
	m_motionState->setWorldTransform(MatTobt(m)); // required for kinematic objects
}
btVector3 btObject::updateKinematic(double delta)
{
	btTransform t;
	//m_motionState->getWorldTransform(t);
	t = m_rigidBody->getWorldTransform();

	btVector3 v = t.getOrigin() - m_oldKinematicTransform.getOrigin();

	m_oldKinematicTransform = t;

	//m_rigidBody->setLinearVelocity(v / delta);
	//m_rigidBody->setLinearVelocity(bt_origin);

	return v / delta;
}
void btObject::reset()
{
	m_rigidBody->clearForces();
	m_rigidBody->setLinearVelocity(btVector3(0, 0, 0));
	m_rigidBody->setAngularVelocity(btVector3(0, 0, 0));
	this->setbtTransform(&m_initialtransform);
	m_rigidBody->activate();
}
void btObject::initObject()
{
	btRigidBody::btRigidBodyConstructionInfo rbci = btRigidBody::btRigidBodyConstructionInfo(m_mass, m_motionState, m_collisionShape, *m_inertia);
	m_rigidBody = new btRigidBody(rbci);

	switch (m_kind)
	{
		case BTOBJECT_INFINITEGROUND:
		{
			//m_rigidBody->setRestitution(0);
			m_rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

			break;
		}
		case BTOBJECT_STATICWORLD:
		{
			//m_rigidBody->setRestitution(0);
			m_rigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
			m_rigidBody->setCcdMotionThreshold(0.15f);
			m_rigidBody->setCcdSweptSphereRadius(0.03f);
			//m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
			break;
		}
		case BTOBJECT_KINEMATICWORLD:
		{
			m_rigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
			m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
			break;
		}
		case BTOBJECT_PLAYER:
		{
			m_rigidBody->setFriction(0);
			//m_rigidBody->setRestitution(0);
			//m_rigidBody->setCcdMotionThreshold(0.15f);
			//m_rigidBody->setCcdSweptSphereRadius(0.03f);
			m_rigidBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
			m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
			m_rigidBody->setAngularFactor(btVector3(0.0f, 1.0f, 0.0f));

			/*btBroadphaseProxy* proxy = phys_obj->rb->getBroadphaseProxy();
			proxy->m_collisionFilterGroup = 1;
			proxy->m_collisionFilterMask = 3;*/
			break;
		}
		case BTOBJECT_CAMERA:
		{
			//m_rigidBody->setFriction(0);
			//m_rigidBody->setGravity(btVector3(0, 0, 0));
			m_rigidBody->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
			m_rigidBody->setActivationState(DISABLE_DEACTIVATION);

			//// 6DOF connected to the world, with motor
			//btTransform frameB;
			//frameB.setIdentity();
			//btGeneric6DofConstraint* pGen6Dof = new btGeneric6DofConstraint(*m_rigidBody, frameB, false);
			//m_world->addConstraint(pGen6Dof);
			//pGen6Dof->setAngularLowerLimit(btVector3(0, 0, 0));
			//pGen6Dof->setAngularUpperLimit(btVector3(0, 0, 0));
			//pGen6Dof->setLinearLowerLimit(btVector3(-10., 0, 0));
			//pGen6Dof->setLinearUpperLimit(btVector3(10., 0, 0));
			//pGen6Dof->getTranslationalLimitMotor()->m_enableMotor[0] = true;
			//pGen6Dof->getTranslationalLimitMotor()->m_targetVelocity[0] = 5.0f;
			//pGen6Dof->getTranslationalLimitMotor()->m_maxMotorForce[0] = 0.1f;
			break;
		}
		default:
			break;
	}

	m_initialtransform = this->getbtTransform();
	m_world->addRigidBody(m_rigidBody);
}
btObject::btObject(unsigned int k, unsigned int p, float m, float3 z, btDefaultMotionState *s, btDiscreteDynamicsWorld *w)
{
	m_world = w;
	m_motionState = s;
	m_kind = k;
	m_primitive = p;
	m_mass = m;
	m_size = z;

	m_collisionShape = getShapeFromPrimitive(m_primitive, m_size);

	m_inertia = new btVector3;
	m_collisionShape->calculateLocalInertia(m_mass, *m_inertia);

	// TODO: join the two constructors (local inertia --> rigidbody is dynamic if and only if mass is non zero, otherwise static)

	initObject();
}
btObject::btObject(unsigned int k, unsigned int p, float m, float3 z, btDefaultMotionState *s, btVector3 *i, btDiscreteDynamicsWorld *w)
{
	m_world = w;
	m_motionState = s;
	m_inertia = i;
	m_kind = k;
	m_primitive = p;
	m_mass = m;
	m_size = z;

	m_collisionShape = getShapeFromPrimitive(m_primitive, m_size);

	initObject();
}
btObject::~btObject()
{
	delete m_motionState;
	delete m_collisionShape;
	delete m_inertia;
	delete m_rigidBody;
}

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
	/*float3 vPoints[2];
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
	device->DrawPrimitiveUP(D3DPT_LINELIST, 1, vPoints, sizeof(float3));*/

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
btVector3 btQuaternionToEuler(const btQuaternion &TQuat)
{
	btScalar W = TQuat.getW();
	btScalar X = TQuat.getX();
	btScalar Y = TQuat.getY();
	btScalar Z = TQuat.getZ();
	float WSquared = W * W;
	float XSquared = X * X;
	float YSquared = Y * Y;
	float ZSquared = Z * Z;

	btVector3 TEuler;

	TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
	TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
	TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
	TEuler *= 180/MATH_PI;

	return TEuler;
};

///

btDiscreteDynamicsWorld* Antimony::btWorld;
std::vector<btObject*> Antimony::physEntities;

std::map<const btCollisionObject*, std::vector<btManifoldPoint*>> Antimony::m_objectsCollisionPoints;
std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> Antimony::m_objectsCollisions;

btDiscreteDynamicsWorld* Antimony::getBtWorld()
{
	return btWorld;
}
void Antimony::tickCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
	m_objectsCollisions.clear();
	m_objectsCollisionPoints.clear();
	int numManifolds = dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold *contactManifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		auto *objA = contactManifold->getBody0();
		auto *objB = contactManifold->getBody1();
		auto& manifoldsA = m_objectsCollisions[objA];
		auto& manifoldsB = m_objectsCollisions[objA];
		manifoldsA.push_back(contactManifold);
		manifoldsB.push_back(contactManifold);
		auto& collisionsA = m_objectsCollisionPoints[objA];
		auto& collisionsB = m_objectsCollisionPoints[objB];
		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; j++)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			collisionsA.push_back(&pt);
			collisionsB.push_back(&pt);
		}
	}
}
void Antimony::staticCallback(btDynamicsWorld *dynamicsWorld, btScalar timeStep)
{
	Antimony::tickCallback(dynamicsWorld, timeStep);
}
void Antimony::addPhysEntity(btObject *obj)
{
	physEntities.push_back(obj);
}
std::vector<btObject*>* Antimony::getEntities()
{
	return &physEntities;
}
void Antimony::resetPhysics()
{
	for (int i = 0; i < physEntities.size(); i++)
		physEntities.at(i)->reset();
}