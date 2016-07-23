#ifndef CHARA_H
#define CHARA_H

#include "Entity.h"

class Character : public Entity
{
protected:

public:
	void update(float delta);

	Character();
};

#endif
