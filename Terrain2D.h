#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "cocos2d.h"
USING_NS_CC;

class TerrainSurface;
class TerrainBottom;

class Terrain2D : public Node
{
public:
	static Terrain2D* create(const std::string &fileName);
	bool init(const std::string &fileName);
	std::vector<Vec2>  getPoints();

private:
	bool loadTerrainData(const std::string &fileName);
	bool generateTerrainData();

private:
	std::vector<Vec2> _points;
	TerrainSurface* _surface;
	TerrainBottom* _bottom;
};

#endif // !__TERRAIN_H__
