#include "Camera.h"
#include "DebugWin.h"

void Camera::update(float delta)
{
	th += th_vel * delta;
	ph += ph_vel * delta;

	th_vel *= friction * delta;
	ph_vel *= friction * delta;

	pos += (pos_dest - pos) * response * 75 * delta;
	lookat += (lookat_dest - lookat) * response * 75 * delta;

	if (pos == pos_dest)
		response = 0.0f;
	if (lookat == lookat_dest)
		response = 0.0f;

	vec3 dir;
	D3DXVec3Normalize(&dir, &(lookat - pos));
	vec3 cross;
	D3DXVec3Cross(&cross, &vec3(0, -1, 0), &dir);

	th = -atan2f(-dir.z, D3DXVec3Dot(&vec3(1, 0, 0), &dir));
	//ph = acosf(D3DXVec3Dot(&vec3(0, -1, 0), &dir) / sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z));
	ph = atan2f(D3DXVec3Length(&cross), D3DXVec3Dot(&vec3(0, -1, 0), &dir));
	//th = asinf(dir.z);
	//ph = asinf(dir.y);
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
	pos = vec3(0, 0, 0);
	pos_dest = vec3(0, 0, 0);
	lookat = vec3(0, 0, 0);
	lookat_dest = vec3(0, 0, 0);
	response = 0.0f;
	friction = 0.99f;
	th = 0.0f;
	th_vel = 0.0f;
	ph = 0.0f;
	ph_vel = 0.0f;
}

Camera camera;