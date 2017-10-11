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

void Character::update(double delta, std::map<const btCollisionObject*, std::vector<btPersistentManifold*>> objectsCollisions)
{
	m_linWorldVel = bt_origin;
	m_angWorldVel = bt_origin;

	asset.update(delta);
	updateContact(JS_NULL);

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
				if (a <= MATH_PI * 0.2)
				{
					// math hocus pocus
					btVector3 d = m_collisionObject->getbtPos() - b->getWorldTransform().getOrigin();
					m_angWorldVel = b->getAngularVelocity();
					m_angWorldVel.setX(0);
					m_angWorldVel.setZ(0);
					m_linWorldVel = b->getLinearVelocity() - d.cross(m_angWorldVel);			// linear vel. is object's vel. plus cross product of angular vel. and distance
					//m_linWorldVel.setY(m_collisionObject->getRigidBody()->getLinearVelocity().getY());
					m_linWorldVel.setY(0);

					/*m_linWorldVel = m_collisionObject->getRigidBody()->getLinearVelocity();
					m_linWorldVel.setX(0);
					m_linWorldVel.setZ(0);*/
					//m_linWorldVel = bt_origin;

					updateContact(JS_CONTACT);

					goto endloop;										// skip all other contact points/manifolds
				}
				//else if (a2 <= MATH_PI * 0.2 && b1 == physEntities.at(0)->rb) // infinite plane has inverted normals (why???)
				//{
				//	player.jumping = 0;
				//	//p = b1->getLinearVelocity();

				//	// no reason to calculate movement here

				//	goto contact;
				//}
				else
				{
					//m_jumping = 1;
					updateContact(JS_FREE);
				}
			}
			//m_jumping = 1;
		}
		updateContact(JS_FREE);
	endloop:;
	}
	else
	{
		updateContact(JS_FREE);
		//m_jumping = 1;
	}

	m_collisionObject->getRigidBody()->setAngularVelocity(btVector3(0, 0, 0));
}
void Character::updateContact(char contact)
{
	if (contact == JS_FREE)				// no contact
	{
		m_jumpState = JUMPSTATE_INAIR;
		asset.animController()->current_anim = 0;
		/*else
		{
			m_jumping = JUMPSTATE_INAIR;
			asset.animController()->current_anim = 4;
		}*/
	}
	if (contact == JS_JUMP)				// actively jumping
	{
		m_jumpState = JUMPSTATE_JUMPING;
		asset.animController()->current_anim = 4;
	}
	else if (contact == JS_CONTACT)		// contact
	{
		if (m_jumpState != JUMPSTATE_ONGROUND)
		{
			m_jumpState = JUMPSTATE_LANDING;
			asset.animController()->current_anim = 4;
		}
	}
	else if (contact == JS_NULL)		// default (no changes)
	{
		if (m_jumpState == JUMPSTATE_JUMPING)
		{
			m_jumpState = JUMPSTATE_INAIR;
			asset.animController()->current_anim = 0;
		}
		else if (m_jumpState == JUMPSTATE_LANDING)
		{
			m_jumpState = JUMPSTATE_ONGROUND;
			asset.animController()->current_anim = 3;
		}
	}
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
	btVector3 d = bt_origin;

	if (((*v) * s) != bt_origin)
	{
		btVector3 orient = bt_origin;

		orient = quatRotate(m_collisionObject->getRigidBody()->getOrientation(), btVector3(0, 0, 1));
		btScalar angle = orient.angle(*v);

		if (btVector3(0, 1, 0).dot(orient.cross(*v)) < 0)
			angle = -angle;
		d = btVector3(0, angle, 0);
	}

	if (m_jumpState == JUMPSTATE_ONGROUND)
	{
		m_collisionObject->getRigidBody()->setLinearVelocity(m_linWorldVel + (*v) * s);		// move with linear velocity when standing on an object
		m_collisionObject->getRigidBody()->setAngularVelocity(m_angWorldVel + (d) * 10);	// rotate to face movement direction
	}
	else
	{
		m_collisionObject->getRigidBody()->applyCentralForce(500 * (*v));					// move using force when not standing on anything
		m_collisionObject->getRigidBody()->setAngularVelocity(m_angWorldVel + (d) * 3);		// rotate to face movement direction (slowly)
	}
}
void Character::attemptJump()
{
	if (m_jumpState != JUMPSTATE_ONGROUND)
		return;

	btVector3 v = btVector3(m_collisionObject->getRigidBody()->getLinearVelocity().getX() * 0.3, m_jumpSpeed, m_collisionObject->getRigidBody()->getLinearVelocity().getZ() * 0.3);
	m_collisionObject->getRigidBody()->setLinearVelocity(v);

	updateContact(JS_JUMP);
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
	return m_collisionObject->getFloat3Pos();// -float3(0, m_collisionObject->getPrimitiveSize().y / 2, 0);
}
mat Character::getTransform()
{
	float y_correction = - m_collisionObject->getPrimitiveSize().y;
	return m_collisionObject->getMatTransform() * XMMatrixTranslation(0, y_correction, 0);
}
bool Character::getMovingState()
{
	return m_moving;
}
char Character::getJumpState()
{
	return m_jumpState;
}
char Character::getActionState()
{
	return m_actionState;
}
Character::Character()
{
	m_moving = false;
	m_jumpState = 0;
	m_actionState = 0;
	//rvel = btVector3(0, 0, 0);
	m_movSpeed = 0.5f;
	m_jumpSpeed = 5;
	m_collisionObject = nullptr;
}
Character::~Character()
{

}