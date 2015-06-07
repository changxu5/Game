#pragma once

#include "cocos2d.h"
#define MOVE_FORCE 200.0
#define JUMP_IMPULSE 22000.0
#define SCALEFACTOR 20.0
#define FIREPOWER_INCREASEMENT 3.0
#define MAX_FIREPOWER 4000.0

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
		this->lastPos = this->getPosition();
		this->fireable = true;
		this->fireAngle = Vec2(1, 1);

		return true;
	}

	void update(float dt)
	{

		auto body = this->getPhysicsBody();
		if (body->getVelocity().length() < 0.1f)
		{
			this->onGround = true;
		}
		else
		{
			this->onGround = false;
		}
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
				this->addingPower = false;
				this->fire();
				this->firepower = 0;
			}
			//log("Firepower : %f", firepower);
		}
		auto child = this->getChildByName("bullet");
		if (child != NULL)
		{
			if (child->getPosition().length() > spr->getContentSize().width/1.5)
			{
				this->saferange = false;
			}
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
		if (onGround)
		{
			this->getPhysicsBody()->applyImpulse(Vec2(0, JUMP_IMPULSE));
		}
	}

	void fire()
	{
		this->addingPower = false;

		auto newBullet = Sprite::create("bullet.png");
		auto bulletBody = PhysicsBody::createBox(newBullet->getContentSize());
		bulletBody->setMass(10.0f);
		bulletBody->setCategoryBitmask(0xFFFFFFFF);
		bulletBody->setContactTestBitmask(0x00000011);
		newBullet->setPhysicsBody(bulletBody);
		newBullet->setName("bullet");
		this->addChild(newBullet);
		this->saferange = true;
		newBullet->setPosition(0, 0);
		newBullet->getPhysicsBody()->setVelocity(this->getPhysicsBody()->getVelocity());
		newBullet->getPhysicsBody()->applyImpulse(firepower * fireAngle);
		this->getPhysicsBody()->applyImpulse(-firepower * fireAngle);
		this->firepower = 0;
		this->fireable = false;
	}
	bool onGround;
	int forced;
	bool addingPower;
	float firepower;
	Vec2 lastPos;
	bool fireable;
	bool saferange;

private:
	
	Sprite* spr;
	Vec2 fireAngle;

};
