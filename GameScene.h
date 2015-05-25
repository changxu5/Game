#pragma once

#include "cocos2d.h"
#include "Player.h"
#include "cocos-ext.h"

USING_NS_CC;

class GameScene : public Layer
{
public:

	CREATE_FUNC(GameScene);
	virtual bool init()
	{
		if (!Layer::init()) return false;
		winSize = Director::getInstance()->getVisibleSize();
		this->setTouchEnabled(true);
		this->addGameBackGround();
		auto objects = map->getObjectGroup("objectLayer1");
		CCASSERT(NULL != objects, "'Objects' object group not found");
		auto spawnPoint = objects->getObject("spawnPoint");
		CCASSERT(!spawnPoint.empty(), "SpawnPoint object not found");
		float x = spawnPoint["x"].asFloat();
		float y = spawnPoint["y"].asFloat();

		player = Player::create();
		auto sprite = Sprite::create("CloseNormal.png");
		player->setSprite(sprite);
		player->setPosition(x, y);
		player->setContentSize(Size(8, 8));

		this->addChild(player, 1);

		layer = map->getLayer("Layer1");

		auto btn = cocos2d::extension::ControlButton::create("JUMP", "fonts/msyh.ttf", 25);
		btn->setPosition(Vec2(winSize.width - btn->getContentSize().width / 2, btn->getContentSize().height * 2));
		btn->addTargetWithActionForControlEvents(player, cccontrol_selector(Player::playerJump), cocos2d::extension::Control::EventType::TOUCH_DOWN);
		this->addChild(btn, 2);

		this->scheduleUpdate();
		return true;
	}
	static cocos2d::Scene * createScene()
	{
		Scene * s = Scene::create();
		Layer * l = GameScene::create();
		s->addChild(l);
		return s;
	}
	void addGameBackGround()
	{
		auto bgspr = Sprite::create("bg.png");
		bgspr->setAnchorPoint(Vec2(0, 0));
		bgspr->setPosition(0, 0);
		this->addChild(bgspr, -1);
		cocos2d::log("LayerPosition :%f %f", this->getPosition().x, this->getPosition().y);

		map = TMXTiledMap::create("1.tmx");
		map->setAnchorPoint(Vec2(0, 0));
		map->setPosition(0, 0);
		this->addChild(map, 0);
	}

	void update(float dt)
	{
		player->update(dt);
		this->checkForAndResolveCollisions(player);
		//this->setViewpointCenter(player->getPosition());
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



private:

	Size winSize;
	TMXTiledMap *map;
	TMXLayer *layer;
	Player *player;

	Point tileCoordForPosition(Point position)
	{
		float x = floor(position.x /map->getTileSize().width); //位置x值/地图一块tile的宽度即可得到x坐标
		float levelHeightInPixels =map->getMapSize().height * map->getTileSize().height; //地图的实际高度
		float y = floor((levelHeightInPixels - position.y) / map->getTileSize().height);  //地图的原点在左上角，与Cocos2d-x是不同的(2dx原点在左下角)
		return Vec2(x, y);
	}
	Rect tileRectFromTileCoords(Point tileCoords)
	{
		float levelHeightInPixels = map->getMapSize().height * map->getTileSize().height; //地图的实际高度
		//把地图坐标tileCoords转化为实际游戏中的坐标
		Point origin = Vec2(tileCoords.x * map->getTileSize().width, levelHeightInPixels - ((tileCoords.y + 1)*map->getTileSize().height));
		return Rect(origin.x, origin.y, map->getTileSize().width, map->getTileSize().height);
	}
	Array* getSurroundingTilesAtPosition(Point position,TMXLayer* layer)
	{
		Point plPos = this->tileCoordForPosition(position); //1 返回此处的tile坐标
		Array* gids = Array::create();
		//3 我们的目的是想取出环绕在精灵四周的8个tile，这里就从上至下每行三个取9个tile（中间一个不算）仔细画画图就知代码的意义
		for (int i = 0; i<9; i++)
		{
			int c = i % 3;   //相当于当前i所处的列
			int r = (int)(i / 3); //相当于当前i所处的行
			Point tilePos = ccp(plPos.x + (c - 1), plPos.y + (r - 1));
			//4 取出包围tile的gid
			int tgid = layer->tileGIDAt(tilePos);
			//5
			Rect tileRect = this->tileRectFromTileCoords(tilePos);  //包围盒
			float x = tileRect.origin.x;  //位置
			float y = tileRect.origin.y;
			//取出这个tile的各个属性，放到CCDictionary里
			Dictionary *tileDict = Dictionary::create();


			String* str_tgid = String::createWithFormat("%d", tgid);
			String* str_x = String::createWithFormat("%f", x);
			String* str_y = String::createWithFormat("%f", y);
			tileDict->setObject(str_tgid, "gid");
			tileDict->setObject(str_x, "x");
			tileDict->setObject(str_y, "y");
			tileDict->setObject((Object *)&tilePos, "tilePos");
			//6
			gids->addObject(tileDict);
		}
		//去掉中间（即自身结点tile）
		gids->removeObjectAtIndex(4);
		gids->insertObject(gids->objectAtIndex(2), 6);
		gids->removeObjectAtIndex(2);
		gids->exchangeObjectAtIndex(4, 6);
		gids->exchangeObjectAtIndex(0, 4);//7
		return gids;
	}
	Rect intersectsRect(const Rect &rectA, const Rect &rectB)
	{
		Point ori;    //左下角顶点     cocos2dx的CCRect是以左下角为原点，w,h为参数(宽和高)，并不是左上角是top,右下角为bottom
		Point topM;	//右下角顶点
		float width = 0.f, height = 0.f;

		if (rectA.intersectsRect(rectB))
		{
			ori.x = max(rectA.getMinX(), rectB.getMinX()); //左下角x坐标取两个矩形ori顶点x坐标大的
			ori.y = max(rectA.getMinY(), rectB.getMinY()); //左下角y坐标取两个矩形ori顶点y坐标大的

			topM.x = min(rectA.getMaxX(), rectB.getMaxX()); //右上角x坐标取两个矩形右上角顶点x坐标小的
			topM.y = min(rectA.getMaxY(), rectB.getMaxY());

			width = topM.x - ori.x;
			height = topM.y - ori.y;

			return Rect(ori.x, ori.y, width, height);
		}
		return Rect(0,0,0,0);
	}
	float max(float a, float b)
	{
		if (a > b) return a;
		else return b;
	}
	float min(float a, float b)
	{
		if (a < b) return a;
		else return b;
	}


	void checkForAndResolveCollisions(Player* player)
	{
		Array* tiles = this->getSurroundingTilesAtPosition(player->getPosition(),layer); //1
		Object* obj = NULL;
		player->onGround = false;
		CCARRAY_FOREACH(tiles, obj)
		{
			Dictionary* dic = (Dictionary*)obj;
			Rect playerRect = player->playerBoundingBox();  //2 玩家的包围盒
			int gid = dic->valueForKey("gid")->intValue();  //3 从CCDictionary中取得玩家附近tile的gid值
			if (gid)
			{
				float rect_x = dic->valueForKey("x")->floatValue();
				float rect_y = dic->valueForKey("y")->floatValue();
				float width = map->getTileSize().width;
				float height = map->getTileSize().height;
				//4 取得这个tile的Rect
				Rect tileRect = Rect(rect_x, rect_y, width, height);
				if (tileRect.intersectsRect(playerRect)) //如果玩家包围盒与tile包围盒相撞
				{
					//5 取得相撞部分
					Rect intersection = this->intersectsRect(playerRect, tileRect);
					int tileIndx = tiles->indexOfObject(dic); //6 取得dic的下标索引

					if (tileIndx == 0)
					{
						//tile在koala正下方 考拉落到了tile上
						player->desirePosition = ccp(player->desirePosition.x, player->desirePosition.y + intersection.size.height);
						player->velocity = Vec2(player->velocity.x, 0);
						player->onGround = true;
					}
					else if (tileIndx == 1) //考拉头顶到tile
					{
						//在koala上面的tile，要让主角向上移移
						player->desirePosition = ccp(player->desirePosition.x, player->desirePosition.y - intersection.size.height);
						player->velocity = Vec2(player->velocity.x, 0);
					}
					else if (tileIndx == 2)
					{
						//左边的tile
						player->desirePosition = ccp(player->desirePosition.x + intersection.size.width, player->desirePosition.y);
					}
					else if (tileIndx == 3)
					{
						//右边的tile
						player->desirePosition = ccp(player->desirePosition.x - intersection.size.width, player->desirePosition.y);
					}
					else
					{
						//7 如果碰撞的水平面大于竖直面，说明角色是上下碰撞
						if (intersection.size.width > intersection.size.height)
						{
							//tile is diagonal, but resolving collision vertically
							player->velocity = ccp(player->velocity.x, 0);

							float intersectionHeight;
							if (tileIndx>5) //说明是踩到斜下的砖块,角色应该向上去
							{
								intersectionHeight = intersection.size.height;
								player->onGround = true;
							}
							else  //说明是顶到斜上的砖块，角色应该向下托
							{
								intersectionHeight = -intersection.size.height;
							}
							player->desirePosition = ccp(player->desirePosition.x, player ->desirePosition.y + intersectionHeight);
						}
						else //如果碰撞的水平面小于竖直面，说明角色是左右撞到
						{
							float resolutionWidth;
							if (tileIndx == 6 || tileIndx == 4) //角色碰到斜左边的tile 角色应该向右去
							{
								resolutionWidth = intersection.size.width;
							}
							else //角色碰到斜右边的tile, 角色应该向左去
							{
								resolutionWidth = -intersection.size.width;
							}
							player->desirePosition = ccp(player->desirePosition.x + resolutionWidth, player->desirePosition.y);
						}
					}
				}
			}
		}
		player->setPosition(player->desirePosition); //7 把主角位置设定到它期望去的地方
	}

	void setViewpointCenter(Point pos)
	{
		//限定角色不能超过半屏
		int x = MAX(pos.x, winSize.width / 2);
		int y = MAX(pos.y, winSize.height / 2);
		//限定角色不能跑出屏幕
		x = MIN(x, (map->getMapSize().width * map->getTileSize().width) - winSize.width / 2);
		y = MIN(y, (map->getMapSize().height * map->getTileSize().height) - winSize.height / 2);
		Point actualPosition = ccp(x, y);
		Point centerOfView = ccp(winSize.width / 2, winSize.height / 2);
		Point viewPoint = ccpSub(centerOfView, actualPosition);
		//设定一下地图的位置
		map->setPosition(viewPoint);
	}





};
