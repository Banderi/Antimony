#pragma once

#include "Entity.h"

///

#define CAM_THETA 0
#define CAM_PHI 1

#define OFF 0
#define ON 1

///

class Camera : public SimpleEntity, public LookAtEntity // TODO: implement camera collisions
{
protected:
	btObject *m_collisionObject;
	btGeneric6DofConstraint *m_collisionConstraint;

	float m_friction;
	float m_th, m_thVel;
	float m_ph, m_phVel;
	float m_quake;

	bool m_dolly_enabled;
	bool m_zoom_enabled;
	bool m_noclip;

public:
	btObject *object;
	float3 displacement;
	float zoom;
	float dolly;
	float minZoom;
	float maxZoom;
	float minDolly;
	float maxDolly;
	float minPitch;
	float maxPitch;

	void update(double delta);

	void enableZoom(bool s);
	void enableDolly(bool s);
	void noclip(bool s);
	bool isZoomEnabled();
	bool isDollyEnabled();
	bool isNoclip();

	void rotate(bool angle, float acceleration);
	void setAngles(float t, float p);
	float getAngle(bool angle);

	void setFriction(float f);
	float getFriction();

	void quake(float t, float q); // TODO: implement quake

	void setCollisionObject(btObject *pc);
	btObject* getColl();

	void setCollisionConstraint(btGeneric6DofConstraint *pc);
	btGeneric6DofConstraint* getConstr();

	Camera();
};