#include "TerrainSurface.h"

TerrainSurface* TerrainSurface::create(std::vector<Vec2>& points)
{
	auto node = new (std::nothrow) TerrainSurface;
	if (node != nullptr)
	{
		auto tex = Director::getInstance()->getTextureCache()->addImage("surface.png");
		node->init(points, tex, "glsl/terrain_surface.vert", "glsl/terrain_surface.frag");
		node->autorelease();
		return node;
	}

	delete node;
	return nullptr;
}

bool TerrainSurface::initTriangles(std::vector<Vec2>& points)
{
	// triangles
	Vec2 pt;
	for (std::size_t i=0; i< points.size(); ++i)
	{
		pt = points[i];
		_triangles.push_back(Vec2(pt.x, pt.y - 16.f));
		_texCoords.push_back(Vec2(pt.x / 256.f, 1.f));
		_triangles.push_back(Vec2(pt.x, pt.y + 16.f));
		_texCoords.push_back(Vec2(pt.x / 256.f, 0.f));
	}
	_bUserTexCoord = true;

	return true;
}
