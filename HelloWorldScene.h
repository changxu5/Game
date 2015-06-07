#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "Player.h"
#include "cocos-ext.h"
#include "Terrain2D.h"

USING_NS_CC;

typedef struct terrainBlock
{
	std::vector<Vec2 > points;
	Node *node;
} TBlock;

class Terrain2D;

class HelloWorld : public Layer
{
public:
    static Scene* createScene();
	virtual bool init();
    CREATE_FUNC(HelloWorld);
	void onEnter();
	void onExit();
private:
	void initUI();
	void initTouch();
	void initKeyBoard();
	void initTerrain();
	void initPhysicsTerrain();
	void addPlayer();
	void setActivePlayer(Player *);
	void setScene(Scene *s);
	void setViewPointCenter(Node * p);
	void setPhysicsWorld(PhysicsWorld * w);
	void removeBulletAndFx();
	void calculateExplode(Point pos, float rad);
	void explodeForce(Point pos,float rad);
	void regenerateTerrain(int pointsGroupNumber);
	void removeTBlock();
	void onTouchesBegan(const std::vector< Touch * > &  touches, Event *  unused_event);
	void onTouchesMoved(const std::vector< Touch * > &  touches, Event *  unused_event);
	void onTouchesEnded(const std::vector< Touch * > &  touches, Event *  unused_event);
	void onKeyPressed(EventKeyboard::KeyCode, Event *);
	void onKeyReleased(EventKeyboard::KeyCode, Event *);
	void jumpaction(Ref * p, cocos2d::extension::Control::EventType e);
	void leftaction(Ref * p, cocos2d::extension::Control::EventType e);
	void rightaction(Ref * p, cocos2d::extension::Control::EventType e);
	void fireaction(Ref * p, cocos2d::extension::Control::EventType e);
	void addplayeraction(Ref * p, cocos2d::extension::Control::EventType e);
	bool onContactBegin(PhysicsContact & contact);
	void onContactSeperate(PhysicsContact & contact);
	void update(float dt);
	void changePlayer(Ref * p, cocos2d::extension::Control::EventType e);
	void setPhysicsDebugMode(bool set);
	void toggledebugaction(Ref * p, cocos2d::extension::Control::EventType e);
	void togglefreeview(Ref * p, cocos2d::extension::Control::EventType e);
private:
	std::vector<TBlock> pointsGroup;
	std::vector<Vec2> circlePoints;
	Terrain2D * _terrain;
	std::vector<Node *> players;
	Player * activePlayer;
	Vec2 visibleSize;
	Size mapsize;
	PhysicsWorld * m_physicsWorld;
	Node * basenode;
	Node * nodeToRemove;
	int pointsGroupToRemove;
	Node * contactTerrainNode;
	Vec2 lastExplodePos;
	bool explode;
	int fxcount;
	bool playerChangable;
	bool debug;
	Camera * camera;
	Scene * m_scene;
	bool freeview;
};

#endif // __HELLOWORLD_SCENE_H__
