#pragma once 

#include "cocos2d.h"
#include "cocos-ext.h"
#include "Player.h"

#define PI 3.141592653

USING_NS_CC;

class GameScene2 : public Layer
{
private:
	Player * activeplayer;
	Node *nodeToRemove;
	int fxcount;
	Point ntrp;
	Vec2 tmppos;
	volatile bool explode;
	bool playerChangable;
	Node * basenode;
	std::vector<Node *> playerlist;
	cocos2d::experimental::TMXTiledMap *map;
	Size Tmapsize;
	Size visibleSize;
	Size originSize;
	short collisionAry[10];

private:
	void initTouch();
	void initKeyBoard();
	void initBackground();
	void addPlayer();
	void initUI();
	void setActivePlayer(Player *p);
	void onTouchesBegan(const std::vector< Touch * > &  touches, Event *  unused_event);
	void onTouchesMoved(const std::vector< Touch * > &  touches, Event *  unused_event);
	void onTouchesEnded(const std::vector< Touch * > &  touches, Event *  unused_event);
	void update(float dt);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void removeTilesOnExploration(Point position, float radius);
	Vec2 worldPositionToTileCord(Point position);
	Point tilePositionToWorldCord(Vec2 cord);
	void setViewPointCenter(Node *p);
	void calculateExplode(Vec2 pos, float radius);
	std::string tileCordToString(Vec2 cord);
	void cleanUpBulletsAndFx();
	void onAndAfterExplode();
	void changePlayer(Ref*, cocos2d::extension::Control::EventType);
	void jumpaction(Ref * p, cocos2d::extension::Control::EventType e);
	void leftaction(Ref * p, cocos2d::extension::Control::EventType e);
	void rightaction(Ref * p, cocos2d::extension::Control::EventType e);
	void fireaction(Ref * p, cocos2d::extension::Control::EventType e);
	bool collisionDetect(Rect orect);
	void sloveCollision();
public:
	CREATE_FUNC(GameScene2);
	static Scene * createScene();
	virtual bool init();
};
