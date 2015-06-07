#ifndef __TERRAIN_BOTTOM_H__
#define __TERRAIN_BOTTOM_H__
#include "TerrainSprite.h"

class TerrainBottom : public TerrainSprite
{
public:
	static TerrainBottom* create(std::vector<Vec2>& points);

protected:
	virtual bool initTriangles(std::vector<Vec2>& points);

};

#endif
