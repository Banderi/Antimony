#pragma once

#include "Entity.h"

class Character : public Entity
{
protected:

public:
	void update(double delta);

	Character();
};
