#include "Camera.h"
#include "DebugWin.h"

void Camera::moveToPoint(vec3 dest, float response)
{
	if (response != -1)
		pos -= (pos - dest) * response;
	else
		pos = dest;
}
void Camera::lookAtPoint(vec3 dest, float response)
{
	if (response != -1)
		lookat -= (lookat - dest) * response;
	else
		lookat = dest;
}
void Camera::setAngle(float t, float p)
{
	if (t != -1)
		theta = t;
	if (p != -1)
		phi = p;
}
vec3 Camera::getPos()
{
	return pos;
}
vec3 Camera::getLookAt()
{
	return lookat;
}
float Camera::getAngle(bool a)
{
	if (a == 0)
		return theta;
	else if (a == 1)
		return phi;
	else
		return -1;
}

Camera::Camera()
{
	pos = vec3(0, 0, 0);
	lookat = vec3(0, 0, 0);
	theta = 0.0f;
	phi = 0.0f;
}

Camera camera;