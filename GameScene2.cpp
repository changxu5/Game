#include <GameScene2.h>



Scene * GameScene2::createScene()
{
	Scene * scene = Scene::createWithPhysics();

	//set Physics debug mode for debug,remeber to remove this
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	scene->getPhysicsWorld()->setSpeed(3.5f);

	GameScene2 * layer = GameScene2::create();
	layer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	scene->addChild(layer);
	return scene;
}

void GameScene2::setActivePlayer(Player *p)
{
	this->activeplayer = p;
}

void GameScene2::initTouch()
{
	//set touch event listener
	auto touchListener = EventListenerTouchAllAtOnce::create();
	touchListener->onTouchesBegan = CC_CALLBACK_2(GameScene2::onTouchesBegan, this);
	touchListener->onTouchesMoved = CC_CALLBACK_2(GameScene2::onTouchesMoved, this);
	touchListener->onTouchesEnded = CC_CALLBACK_2(GameScene2::onTouchesEnded, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void GameScene2::initKeyBoard()
{
	//set keyboard event listener
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = CC_CALLBACK_2(GameScene2::onKeyPressed, this);
	keyboardListener->onKeyReleased = CC_CALLBACK_2(GameScene2::onKeyReleased, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
}

void GameScene2::initBackground()
{
	//init background
	auto bgspr = Sprite::create("bg2.png");
	bgspr->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	basenode->addChild(bgspr, -1);
	bgspr->setPosition(0, 0);

	//init terrain map
	map = cocos2d::experimental::TMXTiledMap::create("6.tmx");
	map->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	basenode->addChild(map, 0);
	map->setPosition(0, 0);
	this->Tmapsize = map->getContentSize();
}

void GameScene2::addPlayer()
{
	//get player spawn point and spawn player
	auto objects = map->getObjectGroup("objectLayer");
	CCASSERT(NULL != objects, "'Objects' object group not found");
	auto spawnPoint = objects->getObject("spawnPoint");
	CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
	Point spawnpos = Vec2(spawnPoint["x"].asFloat(), spawnPoint["y"].asFloat());

	auto player = Player::create();
	player->setName("player1");
	auto playerspr = Sprite::create("CloseNormal.png");
	player->setSprite(playerspr);
	player->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	basenode->addChild(player, 1);
	player->setPosition(spawnpos - Tmapsize/2);
	this->playerlist.push_back(player);
	this->setActivePlayer(player);

	//spawn player2 for test propose
	auto spawnPoint2 = objects->getObject("spawnPoint2");
	CCASSERT(!spawnPoint.empty(), "SpawnPoint2 object not found");
	Point spawnpos2 = Vec2(spawnPoint2["x"].asFloat(), spawnPoint2["y"].asFloat());
	auto player2 = Player::create();
	player2->setName("player2");
	auto player2spr = Sprite::create("CloseNormal.png");
	player2->setSprite(player2spr);
	basenode->addChild(player2, 1);
	player2->setPosition(spawnpos2-Tmapsize/2);
	this->playerlist.push_back(player2);

}

void GameScene2::initUI()
{
	//add UI
	auto btn = cocos2d::extension::ControlButton::create("JUMP", "fonts/msyh.ttf", 25);
	btn->setPosition(Vec2(visibleSize.width - btn->getContentSize().width / 2, btn->getContentSize().height * 2));
	btn->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::jumpaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn, 2);

	auto btn2 = cocos2d::extension::ControlButton::create("Left", "fonts/msyh.ttf", 25);
	btn2->setPosition(Vec2(btn2->getContentSize().width / 2, btn2->getContentSize().height * 3));
	btn2->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::leftaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn2->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::leftaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn2, 2);

	auto btn3 = cocos2d::extension::ControlButton::create("Right", "fonts/msyh.ttf", 25);
	btn3->setPosition(Vec2(btn3->getContentSize().width / 2 + btn2->getContentSize().width, btn3->getContentSize().height * 3));
	btn3->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::rightaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn3->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::rightaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn3, 2);


	auto btn4 = cocos2d::extension::ControlButton::create("Fire", "fonts/msyh.ttf", 25);
	btn4->setPosition(Vec2(visibleSize.width / 2, btn4->getContentSize().height * 3));
	btn4->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::fireaction), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	btn4->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::fireaction), cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE);
	this->addChild(btn4, 2);

	auto btn5 = cocos2d::extension::ControlButton::create("ChangePlayer", "fonts/msyh.ttf", 35);
	btn5->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - btn5->getContentSize().height*2));
	btn5->addTargetWithActionForControlEvents(this, cccontrol_selector(GameScene2::changePlayer), cocos2d::extension::Control::EventType::TOUCH_DOWN);
	this->addChild(btn5, 2);
}

bool GameScene2::init()
{
	if (!Layer::init()) return false;

	//init playerlist
	this->playerlist.clear();
	
	//init node
	this->basenode = Node::create();
	basenode->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	this->addChild(basenode);



	//init varibles
	this->nodeToRemove = NULL;
	this->ntrp = Vec2::ZERO;
	this->fxcount = 0;
	this->tmppos = Vec2::ZERO;
	this->explode = false;
	this->playerChangable = true;
	//init visibleSize and originSize
	visibleSize = Director::getInstance()->getVisibleSize();
	originSize = Director::getInstance()->getVisibleOrigin();

	initTouch();
	initKeyBoard();
	initBackground();
	addPlayer();
	initUI();
	//init update callback
	basenode->setPosition(Tmapsize/2);

	this->scheduleUpdate();
	return true;
}

void GameScene2::update(float dt)
{
	activeplayer->update(dt);
	this->onAndAfterExplode();
	this->cleanUpBulletsAndFx();
	this->setViewPointCenter(activeplayer);
	if (this->collisionDetect(activeplayer->getRect())) this->sloveCollision();
	activeplayer->move();

}

void GameScene2::cleanUpBulletsAndFx()
{
	if (nodeToRemove != NULL && nodeToRemove->getParent() != NULL)
	{
		nodeToRemove->removeFromParentAndCleanup(true);
		nodeToRemove = NULL;
		if (activeplayer->getChildByName("bullet") == NULL)
		{
			activeplayer->fireable = true;
			playerChangable = true;
		}
		while (this->fxcount > 1)
		{
			basenode->removeChildByName("explodefx");
			fxcount--;
		}
	}
}

void GameScene2::onAndAfterExplode()
{
	if (this->explode)
	{
		auto explodefx = ParticleSystemQuad::create("ExplodingRing.plist");
		explodefx->setDuration(0.10f);
		explodefx->setSpeed(explodefx->getSpeed() * 6);
		explodefx->setScale(0.20f);
		explodefx->setLife(0.20f);
		explodefx->setName("explodefx");
		basenode->addChild(explodefx);
		explodefx->setPosition(tmppos);
		this->fxcount++;
		//calculate explode force
		this->calculateExplode(tmppos, 150.0);
		this->removeTilesOnExploration(tmppos, 50.0);
		this->explode = false;
	}
}

void GameScene2::onTouchesBegan(const std::vector< Touch * > &  touches, Event *  unused_event)
{
}

void GameScene2::onTouchesMoved(const std::vector< Touch * > &  touches, Event *  unused_event)
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
	auto nowpos = basenode->getPosition();
	auto newpos = nowpos + touch1->getDelta();
	auto consize = basenode->getScale()*Tmapsize;
	if (newpos.x > consize.x / 2) newpos.x = consize.x / 2;
	if (newpos.x < visibleSize.width - consize.x / 2) newpos.x = visibleSize.width - consize.x / 2;
	if (newpos.y > consize.y / 2) newpos.y = consize.y / 2;
	if (newpos.y < visibleSize.height - consize.y / 2) newpos.y = visibleSize.height - consize.y / 2;
	basenode->setPosition(newpos);

}
void GameScene2::onTouchesEnded(const std::vector< Touch * > &  touches, Event *  unused_event)
{
}

void GameScene2::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode)
	{
		activeplayer->moveto(-1);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode)
	{
		activeplayer->moveto(1);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_UP_ARROW == keyCode)
	{
		return;
	}
	if (EventKeyboard::KeyCode::KEY_SPACE == keyCode)
	{
		activeplayer->jump();
		return;
	}

	if (EventKeyboard::KeyCode::KEY_EQUAL == keyCode)
	{
		basenode->setScale(MAX(1, MIN(2.5, basenode->getScale()*1.1)));
		return;
	}
	if (EventKeyboard::KeyCode::KEY_MINUS== keyCode)
	{
		basenode->setScale(MAX(1, MIN(2.5, basenode->getScale()*0.9)));
		return;
	}
}

void GameScene2::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (EventKeyboard::KeyCode::KEY_LEFT_ARROW == keyCode)
	{
		activeplayer->moveto(0);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_RIGHT_ARROW == keyCode)
	{
		activeplayer->moveto(0);
		return;
	}
	if (EventKeyboard::KeyCode::KEY_SPACE == keyCode)
	{
		return;
	}
}

void GameScene2::jumpaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	activeplayer->jump();
}

void GameScene2::leftaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{
		activeplayer->moveto(-1);
	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{
		activeplayer->moveto(0);
	}
}
void GameScene2::rightaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{
		activeplayer->moveto(1);
	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{
		activeplayer->moveto(0);
	}
}
void GameScene2::fireaction(Ref * p, cocos2d::extension::Control::EventType e)
{
	if (e == cocos2d::extension::Control::EventType::TOUCH_DOWN)
	{

	}
	if (e == cocos2d::extension::Control::EventType::TOUCH_UP_INSIDE)
	{

	}
}

void GameScene2::removeTilesOnExploration(Point position, float radius)
{
	auto layer = map->getLayer("Layer1");
	auto tilesize = map->getTileSize();
	Vec2 tilecord = worldPositionToTileCord(position);
	float maxmapheight = tilesize.height * map->getMapSize().height / 2;
	float maxmapwidth = tilesize.width * map->getMapSize().width / 2;

	Vec2 topleft = Vec2(position.x - radius > -maxmapwidth ? position.x - radius : -maxmapwidth, position.y + radius < maxmapheight ? position.y + radius : maxmapheight);
	Vec2 rightdown = Vec2(position.x + radius < maxmapwidth ? position.x + radius : maxmapwidth, position.y - radius>-maxmapheight ? position.y - radius : -maxmapheight);

	//log("%f %f %f %f", topleft.x, topleft.y, rightdown.x, rightdown.y);
	topleft = worldPositionToTileCord(topleft);
	rightdown = worldPositionToTileCord(rightdown);



	//from topleft to right down
	for (int i = (int)topleft.y; i <= (int)rightdown.y; i++)
	{
		for (int j = (int)topleft.x; j <= (int)rightdown.x; j++)
		{
			//Point origin = Vec2(j * map->getTileSize().width,maxmapheight - ((i + 1)*map->getTileSize().height));
			//origin = origin + Vec2(map->getTileSize().width / 2, map->getTileSize().height / 2);
			Point origin = tilePositionToWorldCord(Vec2(j, i));
			if (sqrtf((origin.x - position.x)*(origin.x - position.x) + (origin.y - position.y)*(origin.y - position.y)) <= radius)
			{
				layer->removeTileAt(Vec2(j, i));
				//log("removing:%d %d", j, i);
				basenode->removeChildByName(tileCordToString(Vec2(j, i)));
				//log("%d %d", j,i);
			}
		}
	}
}

Vec2 GameScene2::worldPositionToTileCord(Point position)
{
	position = position + Vec2(Tmapsize.width / 2, Tmapsize.height / 2);
	float x = floor(position.x / map->getTileSize().width); //位置x值/地图一块tile的宽度即可得到x坐标
	while (x >= map->getMapSize().width) x--;
	while (x < 0) x++;
	float levelHeightInPixels = map->getMapSize().height * map->getTileSize().height; //地图的实际高度
	float y = floor((levelHeightInPixels - position.y) / map->getTileSize().height);  //地图的原点在左上角，与Cocos2d-x是不同的(2dx原点在左下角)
	while (y < 0) y++;
	while (y >= map->getMapSize().height) y--;
	return Vec2(x, y);
}

Point GameScene2::tilePositionToWorldCord(Vec2 cord)
{
	auto tmp = Vec2(cord.x * map->getTileSize().width, map->getTileSize().height * map->getMapSize().height - ((cord.y + 1)*map->getTileSize().height));
	return tmp + Vec2(map->getTileSize().width / 2, map->getTileSize().height / 2) - Vec2(Tmapsize.width / 2, Tmapsize.height / 2);
}

std::string GameScene2::tileCordToString(Vec2 cord)
{
	std::ostringstream s1, s2;
	std::string tmp;
	s1 << cord.x;
	tmp = s1.str();
	s1.flush();
	tmp = tmp + "@";
	s2 << cord.y;
	tmp = tmp + s2.str();
	return tmp;
}


void GameScene2::setViewPointCenter(Node *p)
{
	auto scale = basenode->getScale();
	//Vec2 ppos = p->getPosition()*scale + basenode->getPosition();
	Vec2 ppos = p->convertToWorldSpace(Vec2::ZERO);
	auto screencenter = Vec2(visibleSize.width / 2, visibleSize.height / 2);
	Vec2 diff = (screencenter - ppos);
	auto newpos = basenode->getPosition() + diff;
	auto consize = Tmapsize*scale;
	if (newpos.x > consize.width / 2) newpos.x = consize.width / 2;
	if (newpos.x < visibleSize.width - consize.width / 2) newpos.x = visibleSize.width - consize.width / 2;
	if (newpos.y > consize.height / 2) newpos.y = consize.height / 2;
	if (newpos.y < visibleSize.height - consize.height / 2) newpos.y = visibleSize.height - consize.height / 2;
	auto dif2 = newpos - basenode->getPosition();
	float gate = MIN(MAX(2.0f, dif2.length() / 12), 30);
	if (dif2.length() > gate) dif2 = dif2 / (dif2.length() / gate);
	newpos = basenode->getPosition() + dif2;
	basenode->setPosition(newpos);
}

void GameScene2::calculateExplode(Vec2 pos, float radius)
{
	for (unsigned int i = 0; i < playerlist.size(); i++)
	{
		auto pla = playerlist[i];
		auto dist = pos.distance(pla->getPosition());
		if (dist <= radius)
		{
			//calculate damage here
			//
			//
			//
			auto difvec = pla->getPosition() - pos;
			difvec = difvec / sqrtf(difvec.x * difvec.x + difvec.y * difvec.y);
			if (pla->getPhysicsBody() != NULL)
			{
				pla->getPhysicsBody()->applyImpulse(difvec * 30000.0);
			}
		}
	}
}

void GameScene2::changePlayer(Ref * p , cocos2d::extension::Control::EventType e)
{
	if (!this->playerChangable) return;
	for (int i = 0; i < playerlist.size(); i++)
	{
		if (activeplayer == playerlist[i])
		{
			if ((++i) >= playerlist.size())
			{
				i = 0;
			}
			this->activeplayer = (Player * )playerlist[i];
			return;
		}
	}
}

bool GameScene2::collisionDetect(Rect orect)
{
	auto layer = map->getLayer("Layer1");
	Vec2 lefttop = Vec2(orect.getMinX(), orect.getMaxY());
	Vec2 rightdown = Vec2(orect.getMaxX(), orect.getMinY());
	Vec2 tileSize = map->getTileSize();
	lefttop = worldPositionToTileCord(lefttop);
	rightdown = worldPositionToTileCord(rightdown);

	//init collision Array
	for (int i = 0; i < 10; i++) collisionAry[i] = 0;
	bool retFlag = false;
	for (int i = lefttop.x; i <= rightdown.x; i++)
	{
		for (int j = lefttop.y; j <= rightdown.y; j++)
		{
			if (layer->getTileAt(Vec2(i, j)) == NULL) continue;
			auto tilepos = tilePositionToWorldCord(Vec2(i, j));
			auto tileRect = Rect(tilepos.x - tileSize.x / 2, tilepos.y - tileSize.y / 2, tileSize.x, tileSize.y);
			if (orect.intersectsRect(tileRect))
			{
				retFlag = true;
				if (i == lefttop.x)
				{
					if (j == lefttop.y) 
					{
						collisionAry[1] = 1;
						continue;
					}
					if (j == rightdown.y)
					{
						collisionAry[7] = 1;//左下角
						continue;
					}
					collisionAry[4] = 1;//左
					continue;
				}
				if (i == rightdown.x)
				{
					if (j == lefttop.y)
					{
						collisionAry[3] = 1;
						continue;
					}
					if (j == rightdown.y)
					{
						collisionAry[9] = 1;
						continue;
					}
					collisionAry[6] = 1;
					continue;
				}
				if (j == lefttop.y)
				{
					collisionAry[2] = 1;
					continue;
				}
				if (j == rightdown.y)
				{
					collisionAry[8] = 1;
					continue;
				}
				collisionAry[5] = 1;
				continue;
			}
		}
	}
	return retFlag;
}

void GameScene2::sloveCollision()
{
	//slove coolision
	if (collisionAry[7] || collisionAry[8] || collisionAry[9]) activeplayer->setOnGround(true);
	if (collisionAry[1] || collisionAry[4] || collisionAry[3] || collisionAry[6]) activeplayer->setRanintoWall(true);
	log("%d %d %d %d   %d %d %d %d %d", collisionAry[1], collisionAry[2], collisionAry[3], collisionAry[4], collisionAry[5], collisionAry[6], collisionAry[7], collisionAry[8], collisionAry[9]);
}
