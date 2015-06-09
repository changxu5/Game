#pragma once

#include "cocos2d.h"

#define ACC 500
#define MAX_V_SPEED 500
#define MAX_H_SPEED 200

USING_NS_CC;

class Player : public Node
{
public:

	CREATE_FUNC(Player);

	bool init()
	{
		this->onGround = false;
		this->firepower = 0;
		this->addingPower = false;
		this->fireable = true;
		this->velocity = Vec2::ZERO;
		this->destPos = Vec2::ZERO;

		return true;
	}

	void update(float dt)
	{
		this->calculateMovement(dt);
	}

	void setSprite(Sprite *a)
	{
		this->spr = a;
		this->addChild(spr);
	}

	Sprite * getSprite()
	{
		return this->spr;
	}

	void jump()
	{ 
		if (!onGround) return;
		this->velocity = velocity + Vec2(0,400);
		this->setOnGround(false);
	}

	void fire()
	{
		this->addingPower = false;
		this->firepower = 0;
	}

	Rect getRect()
	{
		auto box = spr->getBoundingBox();
		return Rect(box.getMinX() + this->destPos.x, box.getMinY() + this->destPos.y, spr->getContentSize().width, spr->getContentSize().height);
	}
	void calculateMovement(float dt)
	{
		velocity.y -= ACC * dt;
		if (abs(velocity.y) > MAX_V_SPEED)
		{
			if (velocity.y > 0) velocity.y = MAX_V_SPEED;
			else velocity.y = -MAX_V_SPEED;
		}
		auto mov = velocity *  dt;
		this->destPos = this->getPosition() + mov;
	}
	void move()
	{
		this->setPosition(this->destPos);
	}

	void setOnGround(bool a)
	{
		this->onGround = a;
		if (a)
		{
			this->velocity.y = 0;
			this->destPos.y = this->getPosition().y;
		}
	}

	void setRanintoWall(bool a)
	{
		if (a)
		{
			this->velocity.x = 0;
			this->destPos.x = this->getPositionX();
		}
	}

	void moveto(int direc)
	{
		velocity.x = direc * MAX_H_SPEED;
	}

	bool onGround;
	bool addingPower;
	float firepower;
	bool fireable;
	Vec2 velocity;
	Vec2 destPos;

private:
	
	Sprite* spr;

};
