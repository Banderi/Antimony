#include "Entity.h"

///

#define clamp(x, lower, upper) (min(upper, max(x, lower)))

///

void Entity::lock()
{
	m_free = false;
}
void Entity::unlock()
{
	m_free = true;
}
bool Entity::isFree()
{
	return m_free;
}
Entity::Entity()
{
	m_free = true;
	m_gravity = 0;
}

void SimpleEntity::updatePos(double delta)
{
	float3 mov;

	long double mult;
	long double base;
	long double response;
	long double exponent;

	response = m_posResponse;
	base = 1.0f - response;
	exponent = delta / response;
	mult = 1.0f - pow(base, exponent);

	mov = m_posDest - m_pos;
	m_pos += mov * (float)mult;

	m_fallingSpeed -= m_gravity * delta;
}
void SimpleEntity::moveToPoint(float3 dest, long double r)
{
	m_posDest = dest;
	if (r != -1)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;
		m_posResponse = r;
	}
	else
		m_posResponse = 1;
}
void SimpleEntity::setGravity(float g)
{
	m_gravity = g;
}
float3 SimpleEntity::getPos()
{
	return m_pos;
}
float3 SimpleEntity::getPosDest()
{
	return m_posDest;
}
SimpleEntity::SimpleEntity()
{
	m_pos = float3(0, 0, 0);
	m_posDest = float3(0, 0, 0);
	m_posResponse = 0;
}

void LookAtEntity::updateLookAt(double delta)
{
	float3 mov;

	long double mult;
	long double base;
	long double response;
	long double exponent;

	response = m_lookatResponse;
	base = 1.0f - response;
	exponent = delta / response;
	mult = 1.0f - pow(base, exponent);

	mov = m_lookatDest - m_lookat;
	m_lookat += mov * (float)mult;
}
void LookAtEntity::lookAtPoint(float3 dest, long double r)
{
	m_lookatDest = dest;
	if (r != -1)
	{
		if (r < 0)
			r = 0;
		else if (r > 1)
			r = 1;
		m_lookatResponse = r;
	}
	else
		m_lookatResponse = 1;
}
float3 LookAtEntity::getLookAt()
{
	return m_lookat;
}
float3 LookAtEntity::getLookAtDest()
{
	return m_lookatDest;
}
LookAtEntity::LookAtEntity()
{
	m_lookat = float3(0, 0, 0);
	m_lookatDest = float3(0, 0, 0);
	m_lookatResponse = 0;
}

void PhysEntity::reset()
{
	/*m_coll->rb->clearForces();
	this->m_coll->setMatTransform(&m_initialTransform);*/
}
PhysEntity::PhysEntity(LPCWSTR name, float mass, btCollisionShape *cs, btDefaultMotionState *ms, btDiscreteDynamicsWorld *w)
{
	/*m_coll = nullptr;
	m_world = w;

	btVector3 *in = new btVector3;
	cs->calculateLocalInertia(mass, *in);

	btRigidBody::btRigidBodyConstructionInfo rbci = btRigidBody::btRigidBodyConstructionInfo(mass, ms, cs, *in);
	btRigidBody *rb = new btRigidBody(rbci);
	w->addRigidBody(rb);


	physEntities.push_back(this);

	m_coll = new btObject();
	m_coll->mass = mass;
	m_coll->cs = cs;
	m_coll->ms = ms;
	m_coll->rb = rb;
	m_coll->in = in;

	m_initialtransform = this->GetTransform();*/
}
PhysEntity::PhysEntity(LPCWSTR name, float mass, btCollisionShape *cs, btDefaultMotionState *ms, btVector3 *in, btDiscreteDynamicsWorld *w)
{
	/*m_coll = nullptr;
	m_world = w;

	btRigidBody::btRigidBodyConstructionInfo rbci = btRigidBody::btRigidBodyConstructionInfo(mass, ms, cs, *in);
	btRigidBody *rb = new btRigidBody(rbci);
	m_world->addRigidBody(rb);

	physEntities.push_back(this);

	m_coll = new btObject();
	m_coll->mass = mass;
	m_coll->cs = cs;
	m_coll->ms = ms;
	m_coll->rb = rb;
	m_coll->in = in;

	m_initialtransform = this->GetTransform();*/
}
PhysEntity::~PhysEntity()
{
	//m_world->removeRigidBody(m_coll->rb);
	////delete m_coll->rb->getMotionState();
	//delete m_coll->cs;
	//delete m_coll->ms;
	//delete m_coll->rb;
	//delete m_coll->in;
}

mat CharEntity::getTransform()
{
	btTransform t;
	t = gh->getWorldTransform();
	return btToMat(&t);
}
void CharEntity::setTransform(mat *m)
{
	gh->setWorldTransform(MatTobt(m));
}
void CharEntity::step(double delta)
{
	cc->playerStep(m_world, delta);
}
CharEntity::CharEntity(LPCWSTR name, btPairCachingGhostObject *g, btConvexShape *s, btDiscreteDynamicsWorld *w)
{
	m_world = w;
	gh = g;
	xs = s;

	gh->setCollisionShape(xs);
	//gh->setUserPointer(this);
	gh->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);// | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	gh->setActivationState(DISABLE_DEACTIVATION);
	gh->activate(true);
	cc = new btKinematicCharacterController(gh, xs, 0.2f, btVector3(0, 1, 0));
	cc->setUseGhostSweepTest(false);
	cc->setUpInterpolate(true);
	cc->setJumpSpeed(1);

	w->addCollisionObject(gh, 32, 1 | 2);
	w->addAction(cc);
}
CharEntity::~CharEntity()
{
	m_world->removeCollisionObject(gh);
	delete xs;
	delete gh;
	delete cc;
}