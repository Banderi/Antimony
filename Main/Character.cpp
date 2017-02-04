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
	coll->m_coll->rb->applyCentralForce(btVector3(0, jump_speed, 0));
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
	//

	/*if (GetPos().y <= WORLD_SCALE * 0.6)
		jumping = false;
	else
		jumping = true;*/

	m_collisionObject->rb->setAngularVelocity(btVector3(0, 0, 0));
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
void Character::jump()
{
	if (jumping)
		return;

	btVector3 v = btVector3(m_collisionObject->rb->getLinearVelocity().getX() * 0.3, jump_speed, m_collisionObject->rb->getLinearVelocity().getZ() * 0.3);
	//v.normalize();
	m_collisionObject->rb->setLinearVelocity(v);
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