#include "Terrain2D.h"
#include "TerrainSurface.h"
#include "TerrainBottom.h"

Terrain2D * Terrain2D::create(const std::string &fileName)
{
	auto terrain = new (std::nothrow) Terrain2D;
	if (terrain && terrain->init(fileName))
	{
		terrain->autorelease();
		return terrain;
	}

	delete terrain;
	return nullptr;
}

bool Terrain2D::init(const std::string &fileName)
{
	do 
	{
		CC_BREAK_IF(!Node::init());

		//CC_BREAK_IF(!loadTerrainData(fileName));
		CC_BREAK_IF(!generateTerrainData());

		_bottom = TerrainBottom::create(_points);
		CC_BREAK_IF(_bottom == nullptr);
		this->addChild(_bottom);

		_surface = TerrainSurface::create(_points);
		CC_BREAK_IF(_surface == nullptr);
		this->addChild(_surface);

		return true;
	} while(0);

	return false;
}

bool Terrain2D::loadTerrainData(const std::string &fileName)
{
	auto data = FileUtils::getInstance()->getDataFromFile(fileName);
	if (data.isNull())
	{
		return false;
	}

	int numPoint = *((int*)data.getBytes());

	_points.resize(numPoint);
	memcpy((void *)&_points[0], (void *)(data.getBytes() + 4), data.getSize() - 4);

	return true;
}

bool Terrain2D::generateTerrainData()
{
	int base = 1;
	int height = 120;
	for (int i = 0; i < 100; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		base += 5;
	}
	for (int i = 0; i < 12; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		height += 5;
	}
	for (int i = 0; i < 50; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		base -= 5;
	}

	for (int i = 0; i < 5; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		height += 5;
	}
	for (int i = 0; i < 150; i++)
	{
		Vec2 point(base,height);
		_points.push_back(point);
		base += 5;
	}
	for (int i = 0; i < 15; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		height -= 5;
	}
	for (int i = 0; i < 150; i++)
	{
		Vec2 point(base, height);
		_points.push_back(point);
		base += 5;
	}

	return true;
}

std::vector<Vec2> Terrain2D::getPoints()
{
	return this->_points;
}
