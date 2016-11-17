#include "Camera.h"
#include "DebugWin.h"
#include "Warnings.h"

void Camera::update(double delta)
{
	updatePos(delta);

	th += th_vel * delta;
	ph += ph_vel * delta;

	th_vel *= friction * delta;
	ph_vel *= friction * delta;	

	float3 dir = XMVector3Normalize(lookat - pos);
	float3 n = dir.Cross(float3(0, 1, 0));
	float det = n.Dot(n);

	th = -atan2f(-dir.z, dir.Dot(float3(1, 0, 0)));
	ph = atan2f(n.Dot(n), dir.Dot(float3(0, 1, 0)));
}

void Camera::rotate(bool angle, float accelaration)
{
	if (angle == 0)
		th_vel += accelaration;
	else if (angle == 1)
		ph_vel += accelaration;
	else
		return;
}
void Camera::setAngles(float t, float p)
{
	if (t != -1)
		th = t;
	if (p != -1)
		ph = p;
}
float Camera::getAngle(bool angle)
{
	if (angle == 0)
		return th;
	else if (angle == 1)
		return ph;
	else
		return -1;
}

void Camera::setFriction(float f)
{
	friction = f;
}
float Camera::getFriction()
{
	return friction;
}

Camera::Camera()
{
	free = false;
	pos = float3(0, 0, 0);
	pos_dest = float3(0, 0, 0);
	lookat = float3(0, 0, 0);
	lookat_dest = float3(0, 0, 0);
	pos_response = 0;
	lookat_response = 0;
	friction = 0.99f;
	th = 0.0f;
	th_vel = 0.0f;
	ph = 0.0f;
	ph_vel = 0.0f;
}

Camera camera;