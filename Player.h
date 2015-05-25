#pragma once

#include "cocos2d.h"
#define MOVE_FORCE 200.0
#define JUMP_IMPULSE 30000.0
#define SCALEFACTOR 20.0
#define FIREPOWER_INCREASEMENT 5.0
#define MAX_FIREPOWER 10000.0

USING_NS_CC;

class Player : public Node
{
public:

	CREATE_FUNC(Player);

	bool init()
	{
		this->onGround = false;
		this->forced = 0;
		this->firepower = 0;
		this->addingPower = false;

		return true;
	}

	void update(float dt)
	{
		auto body = this->getPhysicsBody();
		if (this->forced == 1)
		{
			body->applyForce(Vec2(MOVE_FORCE,0));
		}
		else
		{
			if (this->forced == -1)
			{
				body->applyForce(Vec2(-MOVE_FORCE, 0));
			}
			else
			{
				body->resetForces();
			}
		}
		if (this->addingPower == true)
		{
			this->firepower += FIREPOWER_INCREASEMENT * SCALEFACTOR;
			if (this->firepower>MAX_FIREPOWER)
			{
				this->fire();
				this->addingPower = false;
			}
			log("Firepower : %f", firepower);
		}

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
		this->getPhysicsBody()->applyImpulse(Vec2(0,JUMP_IMPULSE));
	}

	void fire()
	{
		this->addingPower = false;
		//
		//
		auto newBullet = Sprite::create("bullet.png");
		auto bulletBody = PhysicsBody::createBox(newBullet->getContentSize());
		bulletBody->setMass(20.0f);
		bulletBody->setCategoryBitmask(0xFFFFFFF0);
		bulletBody->setContactTestBitmask(0x00000001);
		newBullet->setPhysicsBody(bulletBody);
		newBullet->setTag(2);
		this->addChild(newBullet);
		newBullet->setPosition(0, 0);
		newBullet->getPhysicsBody()->applyImpulse(Vec2(firepower / 1.414, firepower / 1.414));
		this->getPhysicsBody()->applyImpulse(Vec2(-firepower / 1.414, -firepower / 1.414));
		//
		this->firepower = 0;
	}

	void startupboost()
	{
		if (forced == 1) this->getPhysicsBody()->applyImpulse(Vec2(1200.0, 0));
		if (forced == -1) this->getPhysicsBody()->applyImpulse(Vec2(-1200.0, 0));
	}

	bool onGround;
	int forced;
	bool addingPower;
	float firepower;

private:
	
	Sprite* spr;

};
