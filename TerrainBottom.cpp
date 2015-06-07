#include "TerrainBottom.h"

TerrainBottom* TerrainBottom::create(std::vector<Vec2>& points)
{
	auto node = new (std::nothrow) TerrainBottom;
	if (node != nullptr)
	{
		auto tex = Director::getInstance()->getTextureCache()->addImage("desert.png");
		node->init(points, tex, "glsl/terrain_bottom.vert", "glsl/terrain_bottom.frag");
		node->autorelease();
		return node;
	}

	delete node;
	return nullptr;
}

bool TerrainBottom::initTriangles(std::vector<Vec2>& points)
{
	// triangles
	Vec2 pt(0.f, 0.f);
	for (std::size_t i=0; i< points.size(); ++i)
	{
		pt.x = points[i].x;
		_triangles.push_back(pt);
		_triangles.push_back(points[i]);
	}
	_bUserTexCoord = false;
	return true;
}
