#ifndef __TERRAIN_SURFACE_H__
#define __TERRAIN_SURFACE_H__
#include "TerrainSprite.h"

class TerrainSurface : public TerrainSprite
{
public:
	static TerrainSurface* create(std::vector<Vec2>& points);

protected:
	virtual bool initTriangles(std::vector<Vec2>& points);
	
};

#endif
