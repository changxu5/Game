#ifndef __TERRAIN_SPRITE_H__
#define __TERRAIN_SPRITE_H__

#include "cocos2d.h"
USING_NS_CC;

class TerrainSprite : public Node
{
public:
	static TerrainSprite* create(std::vector<Vec2>& points, Texture2D* tex, const std::string& vertFile, const std::string& fragFile);
	bool init(std::vector<Vec2>& points, Texture2D* tex, const std::string& vertFile, const std::string& fragFile);
	
	void draw(Renderer *renderer, const Mat4 &transform, uint32_t flags);
	void onDraw(const Mat4 &transform, uint32_t flags);

protected:
	virtual bool initTriangles(std::vector<Vec2>& points) = 0;
	bool initShader(Texture2D* tex, const std::string &vertFile, const std::string& fragFile);	

protected:
	CustomCommand _customCmd;
	std::vector<Vec2> _triangles;
	std::vector<Vec2> _texCoords;
	bool _bUserTexCoord;
};

#endif
