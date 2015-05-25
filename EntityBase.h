#pragma once

#include "cocos2d.h"

USING_NS_CC;

class EntityBase : public Node
{

public:

	CREATE_FUNC(EntityBase);

	bool init()
	{
		return true;
	}
	void setSprite(Sprite *s)
	{
		this->e_sprite = s;
	}
	Sprite * getSprite()
	{
		return this->e_sprite;
	}

private:

	Sprite *e_sprite;


};

