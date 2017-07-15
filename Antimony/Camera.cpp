#include "Camera.h"
#include "Warnings.h"

///

void Camera::update(double delta)
{
	updatePos(delta);
	updateLookAt(delta);

	m_th += m_thVel * delta;
	m_ph += m_phVel * delta;

	m_thVel *= m_friction * delta;
	m_phVel *= m_friction * delta;

	float3 dir = XMVector3Normalize(m_lookat - m_pos);
	float3 n = dir.Cross(float3(0, 1, 0));
	float det = n.Dot(n);

	m_th = -atan2f(-dir.z, dir.Dot(float3(1, 0, 0)));
	m_ph = atan2f(n.Dot(n), dir.Dot(float3(0, 1, 0)));
}
void Camera::rotate(bool angle, float accelaration)
{
	if (angle == 0)
		m_thVel += accelaration;
	else if (angle == 1)
		m_phVel += accelaration;
	else
		return;
}
void Camera::setAngles(float t, float p)
{
	if (t != -1)
		m_th = t;
	if (p != -1)
		m_ph = p;
}
float Camera::getAngle(bool angle)
{
	if (angle == 0)
		return m_th;
	else if (angle == 1)
		return m_ph;
	else
		return -1;
}
void Camera::setFriction(float f)
{
	m_friction = f;
}
float Camera::getFriction()
{
	return m_friction;
}

void Camera::setCollisionObject(btObject *pc)
{
	m_collisionObject = pc;
}
btObject* Camera::getColl()
{
	return m_collisionObject;
}
void Camera::setCollisionConstraint(btGeneric6DofConstraint *pc)
{
	m_collisionConstraint = pc;
}
btGeneric6DofConstraint* Camera::getConstr()
{
	return m_collisionConstraint;
}

Camera::Camera()
{
	m_friction = 0.99f;
	m_th = 0.0f;
	m_thVel = 0.0f;
	m_ph = 0.0f;
	m_phVel = 0.0f;

	zoom = 1;
	displacement = 1;
}