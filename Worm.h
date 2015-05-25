#pragma once

#include "EntityBase.h"

class Worm : public EntityBase
{
public:
	CREATE_FUNC(Worm);
	bool init()
	{

	}
	void setWormName(String name)
	{
		this->wormName = name;
	}

	String getWormName()
	{
		return this->wormName;
	}

private:
	String wormName;
	bool isMoving;



};
