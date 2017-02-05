#include "Character.h"

///

void SimpleCharacter::update(double delta)
{
	updatePos(delta);
	updateLookAt(delta);
}
void SimpleCharacter::updatePhysics(float3 p, float3 d)
{
	m_pos += p;
	m_posDest += d;
}
void SimpleCharacter::jump()
{
	if (!cont->cc->canJump() || !jumping)
		return;


	// add force to collision object
	cont->cc->jump(btVector3(0, jump_speed, 0));
	coll->m_coll->getRigidBody()->applyCentralForce(btVector3(0, jump_speed, 0));
	//coll->m_coll->rb->setLinearVelocity(btVector3(coll->m_coll->rb->getLinearVelocity().getX(), jump_speed, coll->m_coll->rb->getLinearVelocity().getZ()));
}
SimpleCharacter::SimpleCharacter()
{
	jumping = 0;
	action = 0;
	mov_speed = 1.0f;
	jump_speed = 10.0f;
}

void Character::update(double delta)
{
	lin_worldvel = bt_origin;
	ang_worldvel = bt_origin;

	auto& mf = objectsCollisions[m_collisionObject->getRigidBody()];	// get current world manifolds

	if (!mf.empty())
	{
		for (int i = 0; i < mf.size(); i++)								// cycle through manifolds
		{
			auto b = (btRigidBody*)mf.at(i)->getBody1();				// get manifold-ing object
			int numContacts = mf.at(i)->getNumContacts();				// get contact points number
			for (int j = 0; j < numContacts; j++)						// cycle through contact points
			{
				btManifoldPoint& pt = mf.at(i)->getContactPoint(j);		// get contact point
				btVector3 n = pt.m_normalWorldOnB;						// get contact normal
				float a = n.angle(btVector3(0, 1, 0));					// get angle between the normal and the 'up' vector
				if (a <= DX_PI * 0.2)
				{
					jumping = 0;

					// math hocus pocus
					btVector3 d = m_collisionObject->getbtPos() - b->getWorldTransform().getOrigin();
					ang_worldvel = b->getAngularVelocity();
					ang_worldvel.setX(0);
					ang_worldvel.setZ(0);
					lin_worldvel = b->getLinearVelocity() - d.cross(ang_worldvel);			// linear vel. is object's vel. plus cross product of angular vel. and distance

					goto contact;										// skip all other contact points/manifolds
				}
				//else if (a2 <= DX_PI * 0.2 && b1 == physEntities.at(0)->rb) // infinite plane has inverted normals (why???)
				//{
				//	player.jumping = 0;
				//	//p = b1->getLinearVelocity();

				//	// no reason to calculate movement here

				//	goto contact;
				//}
				else
				{
					jumping = 1;
				}
			}
			jumping = 1;
		}
	contact:;
	}
	else
	{
		jumping = 1;
	}

	m_collisionObject->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
}
void Character::warp(Vector3 d)
{
	/*XMFLOAT4X4 t;
	XMStoreFloat4x4(&t, m_collisionObject->getMatTransform());
	t._41 = d.x;
	t._42 = d.y;
	t._43 = d.z;
	m_collisionObject->setMatTransform(&XMLoadFloat4x4(&t));*/

	auto t2 = m_collisionObject->getbtTransform();
	t2.setOrigin(Float3Tobt(&d));
	m_collisionObject->setbtTransform(&t2);
}
void Character::move(btVector3 *v, float s)
{
	if (!jumping)
	{
		m_collisionObject->getRigidBody()->setLinearVelocity(lin_worldvel + (*v) * s);		// move with linear velocity when standing on an object
		m_collisionObject->getRigidBody()->setAngularVelocity(ang_worldvel);
	}
	else
	{
		m_collisionObject->getRigidBody()->applyCentralForce(500 * (*v));					// move using force when not standing on anything
	}
}
void Character::attemptJump()
{
	if (jumping)
		return;

	btVector3 v = btVector3(m_collisionObject->getRigidBody()->getLinearVelocity().getX() * 0.3, jump_speed, m_collisionObject->getRigidBody()->getLinearVelocity().getZ() * 0.3);
	m_collisionObject->getRigidBody()->setLinearVelocity(v);
}
void Character::setCollisionObject(btObject *pc)
{
	m_collisionObject = pc;
}
btObject* Character::getColl()
{
	return m_collisionObject;
}
Vector3 Character::getPos()
{
	return m_collisionObject->getFlat3Pos();
}
Character::Character()
{
	moving = false;
	jumping = 0;
	action = 0;
	//rvel = btVector3(0, 0, 0);
	mov_speed = WORLD_SCALE * 0.5f;
	jump_speed = WORLD_SCALE * 5;
	m_collisionObject = nullptr;
}
Character::~Character()
{

}