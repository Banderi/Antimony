#ifndef CHARA_H
#define CHARA_H

#include "DirectX11.h"

class Character
{
protected:
	vec3 pos;
	vec3 lookat;

public:
	void moveToPoint(vec3 dest, float response);
	void lookAtPoint(vec3 dest, float response);
	vec3 getPos();
	vec3 getLookAt();

	Character();
};

#endif
