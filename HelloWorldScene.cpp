#include "HelloWorldScene.h"

#define PI 3.141592653
#define CIRCLE_DEVIDE_COUNT 30
#define MAX_TBLOCK_POINTSIZE 1000

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
	auto scene = Scene::createWithPhysics();
	//set Physics debug mode for debug,remeber to remove this
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setSpeed(3.5f);
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
	//scene->setCameraMask((int)CameraFlag::USER1,false);

	layer->setScene(scene);

    // return the scene
    return scene;
}

void HelloWorld::setScene(Scene *s)
{
	this->m_scene = s;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //super init first
    if ( !Layer::init() )
    {
        return false;
    }

	//init virables
	this->visibleSize = Director::getInstance()->getVisibleSize();
	//set map size as 1920 * 1080
	this->mapsize = Vec2(1920, 1280);
	this->fxcount = 0;
	this->nodeToRemove = NULL;
	this->pointsGroupToRemove = -1;
	this->lastExplodePos = Vec2::ZERO;
	this->explode = false;
	this->contactTerrainNode = NULL;
	this->debug = false;
	this->freeview = false;
    
	this->initTouch();
	this->initKeyBoard();
	this->initTerrain();
	this->initPhysicsTerrain();
	this->initUI();

	this->scheduleUpdate();
    return true;
}

void HelloWorld::onEnter()
{
	Layer::onEnter();
	camera = Camera::getDefaultCamera();
	this->addPlayer();

}

void HelloWorld::initTouch()
{
	auto touchListener = EventListenerTouchAllAtOnce::create();
	touchListener->onTouchesBegan = CC_CALLBACK_2(HelloWorld::onTouchesBegan, this);
	touchListener->onTouchesMoved = CC_CALLBACK_2(HelloWorld::onTouchesMoved, this);
	touchListener->onTouchesEnded = CC_CALLBACK_2(HelloWorld::onTouchesEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void HelloWorld::initKeyBoard()
{
	_keyboardListener = EventListenerKeyboard::create();
	_keyboardListener->onKeyPressed = CC_CALLBACK_2(HelloWorld::onKeyPressed, this);
	_keyboardListener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

}

void HelloWorld::initTerrain()
{
	_terrain = Terrain2D::create("map.data");
	_terrain->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	//this->addChild(_terrain);
	_terrain->setPosition(0, 0);

	basenode = Node::create();
	basenode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->addChild(basenode);
	basenode->setPosition(0, 0);

	//init background
	auto bgspr = Sprite::create("bg2.png");
	bgspr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	if (bgspr == NULL) return;
	basenode->addChild(bgspr, -1);
	bgspr->setPosition(mapsize/2);
}

void HelloWorld::initPhysicsTerrain()
{
	pointsGroup.clear();
	//init pointsGroup
	TBlock tmp;
	tmp.points = _terrain->getPoints();
	auto pbody = PhysicsBody::createEdgeChain(&tmp.points[0], tmp.points.size(), PHYSICSBODY_MATERIAL_DEFAULT);
	pbody->setCategoryBitmask(0xFFFFFFFF);
	pbody->setContactTestBitmask(0x00000011);
	//bodyt->setDynamic(false);
	auto node = Node::create();
	node->setPhysicsBody(pbody);
	node->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	basenode->addChild(node);
	node->setPosition(0, 0);
	tmp.node = node;
	this->pointsGroup.push_back(tmp);

	//create a large edgebox
	//mapsize.width = tmp.points.back().x - tmp.points.front().x;
	auto edgebody = PhysicsBody::createEdgeBox(mapsize,PHYSICSBODY_MATERIAL_DEFAULT,3.0f);
	edgebody->setCategoryBitmask(0xFFFFFFFF);
	edgebody->setContactTestBitmask(0x00000011);
	auto edgenode = Node::create();
	edgenode->setPhysicsBody(edgebody);
	edgenode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	basenode->addChild(edgenode);
	edgenode->setPosition(mapsize/2);

	//add event listener
	auto _physicsContactListener = EventListenerPhysicsContact::create();
	_physicsContactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
	_physicsContactListener->onContactSeperate = CC_CALLBACK_1(HelloWorld::onContactSeperate, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(_physicsContactListener, this);
}

void HelloWorld::initUI()
{
	//add UI
	auto uiCamera = Camera::create();
	uiCamera->setCameraFlag(CameraFlag::USER1);
	this->addChild(uiCamera);

	auto btn = cocos2d::extension::ControlButton::create("JUMP", "fonts/msyh.ttf", 35);
	btn->setPosition(Vec2(visibleSize.x - btn->getContentSize().width / 2, btn->getContentSize().height * 2));
	btn->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::jumpaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn, 2);
	btn->setCameraMask((int)CameraFlag::USER1, true);

	auto btn2 = cocos2d::extension::ControlButton::create("Left", "fonts/msyh.ttf", 35);
	btn2->setPosition(Vec2(btn2->getContentSize().width / 2, btn2->getContentSize().height * 2));
	btn2->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::leftaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn2->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::leftaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn2, 2);
	btn2->setCameraMask((int)CameraFlag::USER1, true);

	auto btn3 = cocos2d::extension::ControlButton::create("Right", "fonts/msyh.ttf", 35);
	btn3->setPosition(Vec2(btn3->getContentSize().width / 2 + btn2->getContentSize().width*1.5, btn3->getContentSize().height * 2));
	btn3->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::rightaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn3->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::rightaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn3, 2);
	btn3->setCameraMask((int)CameraFlag::USER1, true);


	auto btn4 = cocos2d::extension::ControlButton::create("Fire", "fonts/msyh.ttf", 35);
	btn4->setPosition(Vec2(visibleSize.x-btn4->getContentSize().width, btn4->getContentSize().height * 4));
	btn4->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::fireaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn4->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::fireaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn4, 2);
	btn4->setCameraMask((int)CameraFlag::USER1, true);

	auto btn5 = cocos2d::extension::ControlButton::create("ChangePlayer", "fonts/msyh.ttf", 35);
	btn5->setPosition(Vec2(visibleSize.x /2, visibleSize.y - btn5->getContentSize().height ));
	btn5->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::changePlayer), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn5, 2);
	btn5->setCameraMask((int)CameraFlag::USER1, true);

	auto btn6 = cocos2d::extension::ControlButton::create("AddPlayer", "fonts/msyh.ttf", 35);
	btn6->setPosition(Vec2(visibleSize.x -btn6->getContentSize().width, visibleSize.y - btn6->getContentSize().height));
	btn6->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::addplayeraction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn6, 2);
	btn6->setCameraMask((int)CameraFlag::USER1, true);

	auto btn7 = cocos2d::extension::ControlButton::create("Debug", "fonts/msyh.ttf", 35);
	btn7->setPosition(Vec2(btn7->getContentSize().width/2, visibleSize.y - btn7->getContentSize().height));
	btn7->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::toggledebugaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn7, 2);
	btn7->setCameraMask((int)CameraFlag::USER1, true);

	auto btn8 = cocos2d::extension::ControlButton::create("FreeView", "fonts/msyh.ttf", 35);
	btn8->setPosition(Vec2(btn8->getContentSize().width / 2 + btn7->getContentSize().width*1.2, visibleSize.y - btn8->getContentSize().height));
	btn8->addTargetWithActionForControlEvents(this, cccontrol_selector(HelloWorld::togglefreeview), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn8, 2);
	btn8->setCameraMask((int)CameraFlag::USER1, true);
}

void HelloWorld::addPlayer()
{
	if (this->players.size() > 10) return;

	auto player = Player::create();
	player->setSprite(Sprite::create("CloseNormal.png"));
	auto body = PhysicsBody::createBox(player->getSprite()->getContentSize());
	body->setCategoryBitmask(0xFFFFFFFF);
	body->setContactTestBitmask(0x00000011);
	body->setVelocityLimit(300.f);
	player->setPhysicsBody(body);
	player->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	player->setName("player");
	//_terrain->addChild(player);
	basenode->addChild(player);
	player->setPosition(visibleSize.x / 4 + rand_0_1() * 100, visibleSize.y / 2 + rand_0_1() * 100);
	player->getPhysicsBody()->applyImpulse(Vec2(20, 80));


	this->players.push_back(player);
	this->setActivePlayer(player);
	this->freeview = false;
}

void HelloWorld::setActivePlayer(Player * p )
{
	if (p == NULL) this->activePlayer = NULL;
	this->activePlayer = p;
}

void HelloWorld::setPhysicsWorld(PhysicsWorld * w)
{
	this->m_physicsWorld = w;
}

void HelloWorld::update(float dt)
{
	this->activePlayer->update(dt);
	if(!freeview) this->setViewPointCenter(activePlayer);
	this->removeBulletAndFx();
	this->removeTBlock();

	if (explode)
	{
		this->explodeForce(lastExplodePos, 80.0f);
		this->calculateExplode(lastExplodePos, 40.0f);
	}
}

void HelloWorld::onExit()
{
	Director::getInstance()->getEventDispatcher()->removeEventListener(_touchListener);
	Layer::onExit();
}

void HelloWorld::onTouchesBegan(const std::vector< Touch * > &  touches, Event *  unused_event)
{
	return;
}

void HelloWorld::onTouchesMoved(const std::vector< Touch * > &  touches, Event *  unused_event)
{

	auto touch1 = touches.at(0);
	if (touches.size() > 1)
	{
		auto touch2 = touches.at(1);
		auto point1 = touch1->getLocationInView();
		auto point2 = touch2->getLocationInView();
		auto curdistance = point1.distance(point2);
		auto pridistance = touch1->getPreviousLocation().distance(touch2->getPreviousLocation());
		basenode->setScale(MAX(1, MIN(3, curdistance / pridistance*basenode->getScale())));
	}
	
	auto nowpos = camera->getPosition();
	auto playpos = nowpos - touch1->getDelta();
	auto scale = basenode->getScale();

	if (playpos.x < visibleSize.x / 2) playpos.x = visibleSize.x / 2;
	if (playpos.y < visibleSize.y / 2) playpos.y = visibleSize.y / 2;
	if (playpos.x > mapsize.width*scale - visibleSize.x / 2) playpos.x = mapsize.width*scale - visibleSize.x / 2;
	if (playpos.y > mapsize.height*scale - visibleSize.y / 2) playpos.y = mapsize.height*scale - visibleSize.y / 2;

	
	//auto dif2 = playpos - camera->getPosition();
	//float gate = MIN(MAX(1.5f, dif2.length() / 6), 30);
	//if (dif2.length() > gate) dif2 = dif2 / (dif2.length() / gate);
	//playpos = camera->getPosition() + dif2;
	camera->setPosition(playpos);
	
}

void HelloWorld::onTouchesEnded(const std::vector< Touch * > &  touches, Event *  unused_event)
{
}

void HelloWorld::onKeyPressed(EventKeyboard::KeyCode code, Event *e)
{
	if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == code)
	{
		activePlayer->forced = -1;
		return;
	}
	if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == code)
	{
		activePlayer->forced = 1;
		return;
	}
	if (EventKeyboard::KeyCode::KEY_UP_ARROW == code)
	{
		activePlayer->jump();
		return;
	}
	if (EventKeyboard::KeyCode::KEY_SPACE == code)
	{
		activePlayer->addingPower = true;
		return;
	}
	if (EventKeyboard::KeyCode::KEY_EQUAL == code)
	{
		log("%f %f", basenode->getPositionX(), basenode->getPositionY());
		auto scale = MAX(1.0, MIN(2.5, basenode->getScale()*1.1));
		basenode->setScale(scale);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_MINUS == code)
	{
		auto scale = MAX(1.0, MIN(2.5,basenode->getScale()*0.9));
		basenode->setScale(scale);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_A == code)
	{
		this->addPlayer();
		return;
	}
	if (EventKeyboard::KeyCode::KEY_D == code)
	{
		this->setPhysicsDebugMode(debug);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_C == code)
	{
		this->changePlayer(this, cocos2d::extension::Control::EventType::TOUCH_DOWN);
		return;
	}
}
void HelloWorld::onKeyReleased(EventKeyboard::KeyCode code, Event *e)
{
	if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == code)
	{
		activePlayer->forced = 0;
		return;
	}
	if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == code)
	{
		activePlayer->forced = 0;
		return;
	}
	if (EventKeyboard::KeyCode::KEY_SPACE == code)
	{
		activePlayer->addingPower = false;
		if(activePlayer->fireable && activePlayer->firepower>0) activePlayer->fire();
		else activePlayer->firepower = 0;
		return;
	}
}
void HelloWorld::jumpaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	activePlayer->jump();
}

void HelloWorld::leftaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{
		activePlayer->forced = -1;
	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{
		activePlayer->forced = 0;
	}
}
void HelloWorld::rightaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{
		activePlayer->forced = 1;
	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{
		activePlayer->forced = 0;
	}
}
void HelloWorld::fireaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{
		activePlayer->addingPower = true;
		this->playerChangable = false;
	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{
		activePlayer->addingPower = false;
		if (activePlayer->fireable && activePlayer->firepower>0) activePlayer->fire();
		else activePlayer->firepower = 0;
	}
}

void HelloWorld::changePlayer(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (!this->playerChangable) return;
	for (int i = 0; i < players.size(); i++)
	{
		if (activePlayer == players[i])
		{
			if ((++i) >= players.size())
			{
				i = 0;
			}
			this->activePlayer = (Player *)players[i];
			return;
		}
	}
}

bool HelloWorld::onContactBegin(PhysicsContact & contact)
{
	auto a = contact.getShapeA()->getBody()->getNode();
	auto b = contact.getShapeB()->getBody()->getNode();
	Sprite * c = NULL;
	Node * d = NULL;
	if (a->getName() != "bullet")
	{
		if (b->getName() == "bullet")
		{
			c = (Sprite *)b;
			d = (Node *)a;
			this->contactTerrainNode = a;
		}
	}
	else
	{
		c = (Sprite *)a;
		d = (Node *)b;
		this->contactTerrainNode = b;
	}
	if (c == NULL) return true;
	Node * cp  = c->getParent();
	if (cp == NULL) return true;
	if (nodeToRemove == c) return false;

	if (d == cp)
	{
		if (((Player *)cp)->saferange)
		{
			return true;
		}
	}
	this->nodeToRemove = c;

	auto newpos =( c->convertToWorldSpace(Vec2::ZERO) - basenode->getPosition())/basenode->getScale();
	auto explodefx = ParticleSystemQuad::create("ExplodingRing.plist");
	explodefx->setDuration(0.10f);
	explodefx->setSpeed(explodefx->getSpeed() * 6);
	explodefx->setScale(0.20f);
	explodefx->setLife(0.20f);
	explodefx->setName("explodefx");
	basenode->addChild(explodefx);
	explodefx->setPosition(newpos);
	this->fxcount++;
	explode = true;
	lastExplodePos = newpos;
	return false;
}

void HelloWorld::onContactSeperate(PhysicsContact & contact)
{

}

void HelloWorld::setViewPointCenter(Node *p)
{
	/*
	//auto scale = _terrain->getScale();
	auto scale = basenode->getScale();
	if (mapsize.width*scale <= visibleSize.x && mapsize.height*scale <= visibleSize.y) return;
	//Vec2 ppos = p->getPosition()*scale + _terrain->getPosition();
	Vec2 ppos = p->convertToWorldSpace(Vec2::ZERO);
	auto screencenter = Vec2(visibleSize.x / 2, visibleSize.y / 2);
	Vec2 diff = (screencenter - ppos);
	//auto newpos = _terrain->getPosition() + diff;
	auto newpos = basenode->getPosition() + diff;
	auto consize =mapsize*scale;
	if (newpos.x > 0) newpos.x = 0;
	if (newpos.x <  -(consize.width - visibleSize.x)) newpos.x =-(consize.width - visibleSize.x);
	if (newpos.y > 0) newpos.y = 0;
	if (newpos.y <  -(consize.height - visibleSize.y)) newpos.y = -(consize.height - visibleSize.y);
	//_terrain->setPosition(newpos);

	auto dif2 = newpos - basenode->getPosition();
	float gate = MIN(MAX(1.5f, dif2.length() / 12), 30);
	if (dif2.length() > gate) dif2 = dif2 / (dif2.length() / gate);
	newpos = basenode->getPosition() + dif2;
	basenode->setPosition(newpos);
	*/
	if (activePlayer == NULL) return;
	auto playpos = activePlayer->convertToWorldSpace(Vec2::ZERO);
	auto scale = basenode->getScale();

	if (playpos.x < visibleSize.x/2) playpos.x = visibleSize.x/2;
	if (playpos.y < visibleSize.y/2) playpos.y = visibleSize.y/2;
	if (playpos.x > mapsize.width*scale - visibleSize.x / 2) playpos.x = mapsize.width*scale - visibleSize.x / 2;
	if (playpos.y > mapsize.height*scale - visibleSize.y / 2) playpos.y = mapsize.height*scale - visibleSize.y / 2;

	auto dif2 = playpos - camera->getPosition();
	float gate = MIN(MAX(1.5f, dif2.length() / 12), 30);
	if (dif2.length() > gate) dif2 = dif2 / (dif2.length() / gate);
	playpos = camera->getPosition() + dif2;
	camera->setPosition(playpos);
}

void HelloWorld::removeBulletAndFx()
{
	this->playerChangable = true;
	if (nodeToRemove != NULL && nodeToRemove->getParent() != NULL)
	{
		nodeToRemove->removeFromParentAndCleanup(true);
		nodeToRemove = NULL;
		if (activePlayer->getChildByName("bullet") == NULL) activePlayer->fireable = true;		
	}
	while (this->fxcount > 1)
	{
		basenode->removeChildByName("explodefx");
		fxcount--;
	}
	////log("%d %d %d", this->getChildrenCount(), basenode->getChildrenCount(), activePlayer->getChildrenCount());
}

void HelloWorld::calculateExplode(Point pos, float rad)
{
	//确定爆炸影响到的是哪一个点组
	pos = pos  + basenode->getPosition();
	explode = false;
	std::vector<Vec2> points;
	int pointGroupNumber = -1;
	for (int i = 0; i < pointsGroup.size(); i++)
	{
		if (pointsGroup[i].node == this->contactTerrainNode)
		{
			points = (pointsGroup[i].points);
			pointGroupNumber = i;
			//log("PointGroup :size :%d", points.size());
			break;
		}
	}
	if (pointGroupNumber == -1)
	{
		//log("fatal error!");
		return;
	}
	////log("explode at pos:%f %f", pos.x, pos.y);
	circlePoints.clear();
	pos = pos - basenode->getPosition();
	float angle = 0;
	//计算爆炸圆点组
	int circleFlag[CIRCLE_DEVIDE_COUNT + 1] = { 0 }, tmpAry[CIRCLE_DEVIDE_COUNT + 1] = {0};
	for (int i = 0; i <= CIRCLE_DEVIDE_COUNT; i++) circleFlag[i] = 0;
	while (1)
	{
		if (angle >= 2 * PI) break;
		Vec2 tpoint(pos + rad*Vec2(cos(angle), sin(angle)));
		if (tpoint.x < 0) tpoint.x = 0;
		if (tpoint.y < 0) tpoint.y = 0;
		if (tpoint.x > mapsize.width) tpoint.x = mapsize.width;
		if (tpoint.y > mapsize.height) tpoint.y = mapsize.height;
		circlePoints.push_back(tpoint);
		angle += 2 * PI / CIRCLE_DEVIDE_COUNT;
	}

	int flag = 0;
	std::vector<Vec2> c,d,f,h,newpoints;
	std::vector<int> e,g;
	Vec2 tmp(0, 0);
	bool everout = false;
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].distance(pos) < rad)
		{
			if (flag == 0)
			{
				tmp.x = i;
				flag = 1;
			}
			else
			{
				tmp.y = i;
			}
		}
		else
		{
			everout = true;
			if (flag == 1)
			{
				if (tmp.x >= tmp.y || tmp.y == 0 || (int)tmp.y-(int)tmp.x == 1)
				{
					flag = 0;
					tmp = Vec2::ZERO;
					continue;
				}
				c.push_back(tmp);
				flag = 0;
				tmp = Vec2::ZERO;
			}
		}
	}
	if (tmp != Vec2::ZERO && tmp.y != 0) c.push_back(tmp);

	if (c.empty()) return;
	if (!everout || (c[0].x == 0 && c[0].y == points.size() - 1))
	{
		this->pointsGroupToRemove = pointGroupNumber;
	}
	//以上计算爆炸影响到的地形界点
	//记录的是点编号而非点坐标
	//log("Terrain points:");
	for (int i = 0; i < c.size(); i++)
	{
		//log("%f %f", c[i].x, c[i].y);
	}

	d.clear();
	for (int j = 0; j < c.size();)
	{
		int min1 = circlePoints[0].distance(points[c[j].x]);
		int min2 = circlePoints[0].distance(points[c[j].y]);
		tmp = Vec2::ZERO;
		for (int i = 0; i < circlePoints.size(); i++)
		{
			if (circlePoints[i].distance(points[c[j].x]) <= min1)
			{
				min1 = circlePoints[i].distance(points[c[j].x]);
				tmp.x = i;
			}
			if (circlePoints[i].distance(points[c[j].y]) <= min2)
			{
				min2 = circlePoints[i].distance(points[c[j].y]);
				tmp.y = i;
			}
		}
		if (++(tmp.y) >= circlePoints.size()) tmp.y = 0;
		if (tmp.x == tmp.y)
		{
			c.erase(c.begin() + j);
		}
		else
		{
			d.push_back(tmp);
			j++;
		}
	}
	//log("vec d:");
	for (int i = 0; i < d.size(); i++)
	{
		//log("%f %f", d[i].x, d[i].y);
	}
	//以上计算地形界点与爆炸圆交点的点编号

	for (int i = 0; i < d.size(); i++)
	{
		circleFlag[(int)d[i].x] = 1;
		circleFlag[(int)d[i].y] = 1;
	}

	//circling from start point
	e.clear();
	if (d.empty()) return;
	for (int i = d[0].x,k=0;k<circlePoints.size();k++)
	{
		if (circleFlag[i]) e.push_back(i);
		i++;
		if (i >= circlePoints.size()) i = 0;
	}
	//log("int e:");
	for (int i = 0; i <e.size(); i++) //log("%d", e[i]);
	if (e.size() % 2) return;
	
	f.clear();
	for (int i = 0; i < e.size();i+=2)
	{
		f.push_back(Vec2(e[i], e[i+1]));
	}
	//log("Vec f:");
	for (int i = 0; i < f.size(); i++) //log("%f %f", f[i].x, f[i].y);
	if (f.empty()) return;

	g.clear();
	Vec2 tmp2 = Vec2::ZERO;
	for (int i = 0; i < e.size(); i++)
	{
		for (int j = 0; j < d.size(); j++)
		{
			if (d[j].x == e[i]) g.push_back(c[j].x);
			else
			{
				if (d[j].y == e[i]) g.push_back(c[j].y);
			}

		}
	}
	//log("int g:");
	for (int i = 0; i < g.size(); i++) //log("%d", g[i]);

	h.clear();
	for (int i = 0; i < g.size(); i += 2)
	{
		h.push_back(Vec2(g[i], g[i + 1]));
	}
	//log("Vec h:");
	for (int i = 0; i < h.size(); i++) //log("%f %f", h[i].x, h[i].y);
	if (h.empty()) return;


	char bitmap[MAX_TBLOCK_POINTSIZE] = {0};
	for (int i = 0; i < h[0].x; i++) newpoints.push_back(points[i]);
	int current = h[0].x;
	for (int i = 0; i < h.size(); i++)
	{
		if (bitmap[(int)h[i].x] == 0 && bitmap[(int)h[i].y] == 0)
		{
			if (h[i].x < h[i].y)
			{
				//log("1");
				while (current < h[i].x)
				{
					//log("current = %d", current);
					newpoints.push_back(points[current]);
					current++;
					bitmap[current] = 1;
				}
				//log("1");
				for (int k = h[i].x; k < h[i].y; k++) bitmap[k] = 1;
				current = h[i].y;
				//log("1");
				for (int k = f[i].x; k != f[i].y; k++)
				{
					if (k >= circlePoints.size()) k = 0;
					newpoints.push_back(circlePoints[k]);
				}
				//log("1");
				continue;
			}
			else
			{
				//log("Error!");
				return;
			}

		}
		else
		{
			//log("new terrain!");
			if (bitmap[(int)h[i].x] == 1 && bitmap[(int)h[i].y] == 1)
			{
				if (h[i].x > h[i].y)
				{
					TBlock newterrain;
					std::vector<Vec2> newterrainpoints;
					for (int k = h[i].y; k <= h[i].x; k++) newterrainpoints.push_back(points[k]);
					for (int k = f[i].x; k != f[i].y; k++)
					{
						if (k >= circlePoints.size()) k = 0;
						newterrainpoints.push_back(circlePoints[k]);
					}
					newterrain.points = newterrainpoints;
					auto pbody = PhysicsBody::createEdgeChain(&newterrainpoints[0], newterrainpoints.size(), PHYSICSBODY_MATERIAL_DEFAULT);
					pbody->setCategoryBitmask(0xFFFFFFFF);
					pbody->setContactTestBitmask(0x00000011);
					//bodyt->setDynamic(false);
					auto newnode = Node::create();
					newnode->setPhysicsBody(pbody);
					newnode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
					//_terrain->addChild(node);
					basenode->addChild(newnode);
					newnode->setPosition(0, 0);
					newterrain.node = newnode;
					this->pointsGroup.push_back(newterrain);
				}
				else
				{
					//log("Error!");
					return;
				}
			}
		}
	}
	
	//log("completed.");
	for (int i = c.back().y; i < points.size(); i++) newpoints.push_back(points[i]);

	pointsGroup[pointGroupNumber].points = newpoints;

	this->regenerateTerrain(pointGroupNumber);
}

void HelloWorld::regenerateTerrain(int pointsGroupNumber)
{
	//log("%d", pointsGroupNumber);
	auto points = pointsGroup[pointsGroupNumber].points;
	if (points.empty())
	{
		this->pointsGroupToRemove = pointsGroupNumber;
		return;
	}
	auto pbody = PhysicsBody::createEdgeChain(&points[0],points.size(), PHYSICSBODY_MATERIAL_DEFAULT);
	pbody->setCategoryBitmask(0xFFFFFFFF);
	pbody->setContactTestBitmask(0x00000011);
	contactTerrainNode->setPhysicsBody(pbody);
}

void HelloWorld::removeTBlock()
{
	if (pointsGroupToRemove != -1)
	{
		pointsGroup[pointsGroupToRemove].node->removeFromParentAndCleanup(true);
		this->pointsGroup.erase(pointsGroup.begin() + pointsGroupToRemove);
		pointsGroupToRemove = -1;
	}
}

void HelloWorld::addplayeraction(Ref * p, cocos2d::extension::Control::EventType e)
{
	this->addPlayer();
}

void HelloWorld::setPhysicsDebugMode(bool set)
{
	if (!set) this->m_physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_SHAPE);
	else this->m_physicsWorld->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_NONE);
}

void HelloWorld::toggledebugaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (debug) this->setPhysicsDebugMode(false);
	else this->setPhysicsDebugMode(true);

	debug = !debug;
}

void HelloWorld::togglefreeview(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (this->freeview) this->freeview = false;
	else this->freeview = true;
}

void HelloWorld::explodeForce(Point pos, float rad)
{
	for (int i = 0; i < players.size(); i++)
	{
		auto ppos = players[i]->getPosition()+basenode->getPosition();
		if (ppos.distance(pos) < rad)
		{
			auto diff = ppos - pos;
			diff.x = 100 / diff.x;
			diff.y = 100 / diff.y;
			if (abs(diff.x) > 15)
			{
				if (diff.x < 0) diff.x = -15;
				else diff.x = 15;
			}
			if (abs(diff.y) > 15)
			{
				if (diff.y < 0) diff.y = -15;
				else diff.y = 15;
			}
			auto force = diff * 2000;
			log("%f", force.length());
			players[i]->getPhysicsBody()->applyImpulse(force);
		}
	}
}
