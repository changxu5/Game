#pragma once 

#include "cocos2d.h"
#include "Player.h"
#define PI 3.141592653

USING_NS_CC;

//对于节点的tag值
//player 3
//bullets 2
//exlodefx 4

class GameScene2 : public Layer
{
private:
	PhysicsWorld * m_world;
	Player * player;
	int keypressed;
	Node *nodeToRemove;
	int fxcount;
	Point ntrp;
	Vec2 tmppos;
	volatile bool explode;

	void setPhysicsWorld(PhysicsWorld * world)
	{
		this->m_world = world;
	}


public:
	TMXTiledMap *map;
	Size visibleSize;

	CREATE_FUNC(GameScene2);

	static Scene * createScene()
	{
		Scene * scene = Scene::createWithPhysics();

		//set Physics debug mode for debug,remeber to remove this
		//scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
		scene->getPhysicsWorld()->setSpeed(3.5f);

		GameScene2 * layer = GameScene2::create();
		layer->setPhysicsWorld(scene->getPhysicsWorld());

		scene->addChild(layer);
		return scene;
	}

	virtual bool init()
	{
		if (!Layer::init()) return false;

		//init varibles
		this->keypressed = 0;
		this->nodeToRemove = NULL;
		this->ntrp = Vec2::ZERO;
		this->fxcount = 0;
		this->tmppos = Vec2::ZERO;
		this->explode = false;

		//init visibleSize
		visibleSize = Director::getInstance()->getVisibleSize();

		//init update callback
		this->scheduleUpdate();

		//set touch enabled
		this->setTouchEnabled(true);

		//set keyboard event listener
		auto keyboardListener = EventListenerKeyboard::create();
		keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene2::onKeyPressed, this);
		keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene2::onKeyReleased, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

		//set physics contact event listener
		auto conntactListener = EventListenerPhysicsContact::create();
		conntactListener->onContactBegin = CC_CALLBACK_1(GameScene2::onPyhsicsContact, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(conntactListener, this);


		//init background
		auto bgspr = Sprite::create("bg.png");
		bgspr->setAnchorPoint(Vec2(0, 0));
		bgspr->setPosition(0, 0);
		this->addChild(bgspr, -1);

		//init terrain
		map = TMXTiledMap::create("3.tmx");
		map->setAnchorPoint(Vec2(0, 0));
		map->setPosition(0, 0);
		this->addChild(map, 0);

		//physics edge
		auto body = PhysicsBody::createEdgeBox(visibleSize, PHYSICSBODY_MATERIAL_DEFAULT, 3);
		body->setCategoryBitmask(0xFFFFFFFF);
		body->setContactTestBitmask(0x00000011);
		auto edgeNode = Node::create();
		edgeNode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
		edgeNode->setPosition(Point(visibleSize.width / 2, visibleSize.height / 2));
		edgeNode->setPhysicsBody(body);
		this->addChild(edgeNode);

		//get player spawn point and spawn player
		auto objects = map->getObjectGroup("objectLayer");
		CCASSERT(NULL != objects, "'Objects' object group not found");
		auto spawnPoint = objects->getObject("spawnPoint");
		CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
		Point spawnpos = Vec2(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());

		player = Player::create();
		player->setTag(3); //player tag's 3

		auto playerspr = Sprite ::create("CloseNormal.png");

		auto pbody = PhysicsBody::createBox(playerspr->getContentSize());
		pbody->setCategoryBitmask(0xFFFFFFF0);
		pbody->setContactTestBitmask(0x00000000);
		pbody->setVelocityLimit(300.f);
		pbody->setVelocity(Vec2(220, 30));

		player->setPhysicsBody(pbody);
		player->setSprite(playerspr);
		player->setPosition(spawnpos);

		this->addChild(player, 1);


		//init physics terrain for collision
		/*
		auto tobjects = objects->getObjects();
		for (int it = 0; it < tobjects.size(); it++)
		{
			auto t1 = tobjects[it].asValueMap();
			float tx = t1["x"].asFloat();
			float ty = t1["y"].asFloat();
			float tw = t1["width"].asFloat();
			float th = t1["height"].asFloat();
			auto bodyt = PhysicsBody::createEdgeBox(Size(tw, th));
			bodyt->setCategoryBitmask(0xFFFFFFFF);
			bodyt->setContactTestBitmask(0x00000011);
			//bodyt->setDynamic(false);
			auto edgeNodet = Node::create();
			edgeNodet->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
			edgeNodet->setPosition(Point(tx + tw / 2, ty + th / 2));
			edgeNodet->setPhysicsBody(bodyt);
			this->addChild(edgeNodet);
		}
		*/
		//初始化物理世界，使用每一个图块生成一个刚体的方式。
		//可能会对性能造成一定影响，待验证
		auto terlay =  map->getLayer("Layer1");
		//遍历地图上所有tiles
		auto mapsize = map->getMapSize();
		for (int i = 0; i < mapsize.width; i++)
		{
			for (int j = 0; j < mapsize.height; j++)
			{
				if (terlay->tileAt(Vec2(i, j)) != NULL)
				{
					auto bodyt = PhysicsBody::createEdgeBox(Size(map->getTileSize().width, map->getTileSize().height));
					bodyt->setCategoryBitmask(0xFFFFFFFF);
					bodyt->setContactTestBitmask(0x00000011);
					//bodyt->setDynamic(false);
					auto edgeNodet = Node::create();
					edgeNodet->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
					edgeNodet->setPosition(tilePositionToWorldCord(Vec2(i,j)));
					edgeNodet->setPhysicsBody(bodyt);
					edgeNodet->setName(tileCordToString(Vec2(i, j)));
					log("Node name:%s", edgeNodet->getName().c_str());
					this->addChild(edgeNodet);
				}
			}
		}

		return true;
	}


	void onTouchesBegan(const std::vector< Touch * > &  touches, Event *  unused_event)
	{
		auto touch = touches.at(0);
		float xp = touch->getLocation().x;
		if (xp > player->getPosition().x) player->forced = 1;
		else player->forced = -1;
	}

	void onTouchesMoved(const std::vector< Touch * > &  touches, Event *  unused_event)
	{
		auto touch = touches.at(0);
		float xp = touch->getLocation().x;
		if (xp > player->getPosition().x) player->forced = 1;
		else player->forced = -1;
	}
	void onTouchesEnded(const std::vector< Touch * > &  touches, Event *  unused_event)
	{
		player->forced = 0;
	}
	void update(float dt)
	{
		if (nodeToRemove != NULL && nodeToRemove->getParent()!=NULL)
		{
			nodeToRemove->removeFromParentAndCleanup(true);
			nodeToRemove = NULL;
			while (this->fxcount > 1)
			{
				this->removeChildByTag(4);
				fxcount--;
			}
			log("%d", this->getChildrenCount());
		}
		if (this->explode == true)
		{
			this->removeTilesOnExploration(tmppos, 50.0);
			this->explode = false;
		}
		player->update(dt);

	}


	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
	{
		this->keypressed += 1;
		if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode)
		{
			this->player->forced = -1;
			this->player->startupboost();
			return;
		}
		if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode)
		{
			this->player->forced = 1;
			this->player->startupboost();
			return;
		}
		if (EventKeyboard::KeyCode::KEY_UP_ARROW == keyCode)
		{
			this->player->jump();
			return;
		}
		if (EventKeyboard::KeyCode::KEY_SPACE == keyCode)
		{
			this->player->addingPower = true;
		}
	}

	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
	{
		this->keypressed -= 1;
		if (EventKeyboard::KeyCode::KEY_SPACE == keyCode)
		{
			if (this->player->addingPower == true)
			{
				this->player->fire();
			}
			this->player->addingPower = false;
			
		}
		if (this->keypressed==0) this->player->forced = 0;
	}

	bool onPyhsicsContact(PhysicsContact& contact)
	{
		//获取碰撞的两个节点
		//log("Contact detected");
		auto a = contact.getShapeA()->getBody()->getNode();
		auto b = contact.getShapeB()->getBody()->getNode();
		Sprite * c = NULL;
		if (a->getTag() != 2)
		{
			if (b->getTag() == 2)
			{
				c = (Sprite *)b;
			}
		}
		else
		{
			c = (Sprite *)a;
		}
		this->nodeToRemove = c;

		if (c == NULL || c->getParent() == NULL) return true;

		auto nodep = nodeToRemove->getParent();

		auto t1 = nodep->getPosition();
		auto t2 = nodeToRemove->getPosition();
		auto t3 = nodep->getRotation() / 180 * PI;
		auto newpos = Vec2(t2.x * cos(t3) + t2.y * sin(t3), -t2.x * sin(t3) + t2.y * cos(t3));
		newpos = newpos + t1;
		this->tmppos = newpos;
		this->explode = true;

		//auto explodefx = ParticleExplosion::create();
		//add explode fx
		auto explodefx = ParticleSystemQuad::create("ExplodingRing.plist");
		explodefx->setDuration(0.10f);
		explodefx->setSpeed(explodefx->getSpeed()*3);
		explodefx->setScale(0.20f);
		explodefx->setLife(0.20f);
		explodefx->setTag(4);
		explodefx->setPosition(newpos);
		this->addChild(explodefx);
		this->fxcount++;
		
		//log("%d", this->getChildrenCount());

		return true;
	}

	void removeTilesOnExploration(Point position, float radius)
	{
		auto layer = map->getLayer("Layer1");
		auto tilesize = map->getTileSize();
		Vec2 tilecord = worldPositionToTileCord(position);
		float maxmapheight = tilesize.height * map->getMapSize().height;
		float maxmapwidth = tilesize.width * map->getMapSize().width;

		Vec2 topleft = Vec2(position.x - radius > 0 ? position.x - radius : 0, position.y + radius < maxmapheight ? position.y + radius : maxmapheight);
		Vec2 rightdown = Vec2(position.x + radius < maxmapwidth ? position.x + radius : maxmapwidth, position.y - radius>0 ? position.y - radius : 0);
		
		//log("%f %f %f %f", topleft.x, topleft.y, rightdown.x, rightdown.y);
		topleft = worldPositionToTileCord(topleft);
		rightdown = worldPositionToTileCord(rightdown);



		//from topleft to right down
		for (int i = (int)topleft.y; i <= (int)rightdown.y; i++)
		{
			for (int j = (int)topleft.x; j <= (int)rightdown.x; j++)
			{
				Point origin = Vec2(j * map->getTileSize().width,maxmapheight - ((i + 1)*map->getTileSize().height));
				origin = origin + Vec2(map->getTileSize().width / 2, map->getTileSize().height / 2);
				if (sqrtf((origin.x - position.x)*(origin.x - position.x) + (origin.y - position.y)*(origin.y - position.y) )<= radius)
				{
					layer->removeTileAt(Vec2(j, i));
					log("removing:%d %d", j, i);
					this->removeChildByName(tileCordToString(Vec2(j, i)));
					//log("%d %d", j,i);
				}
			}
		}
	}
	Vec2 worldPositionToTileCord(Point position)
	{
		float x = floor(position.x / map->getTileSize().width); //位置x值/地图一块tile的宽度即可得到x坐标
		while (x >= map->getMapSize().width) x--;
		while (x < 0) x++;
		float levelHeightInPixels = map->getMapSize().height * map->getTileSize().height; //地图的实际高度
		float y = floor((levelHeightInPixels - position.y) / map->getTileSize().height);  //地图的原点在左上角，与Cocos2d-x是不同的(2dx原点在左下角)
		while (y < 0) y++;
		while (y >= map->getMapSize().height) y--;
		return Vec2(x, y);
	}

	Point tilePositionToWorldCord(Vec2 cord)
	{
		auto tmp = Vec2(cord.x * map->getTileSize().width, map->getTileSize().height * map->getMapSize().height - ((cord.y + 1)*map->getTileSize().height));
		return tmp + Vec2(map->getTileSize().width / 2, map->getTileSize().height / 2);
	}

	std::string tileCordToString(Vec2 cord)
	{
		std::ostringstream s1,s2;
		std::string tmp;
		s1 << cord.x;
		tmp = s1.str();
		s1.flush();
		tmp = tmp + "@";
		s2 << cord.y;
		tmp = tmp + s2.str();
		return tmp;
	}

};
