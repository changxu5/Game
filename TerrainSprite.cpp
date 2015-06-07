#include "TerrainSprite.h"

bool TerrainSprite::initShader(Texture2D* tex, const std::string &vertFile, const std::string& fragFile)
{
	// texture
	Texture2D::TexParams texParams;
	texParams.magFilter = GL_LINEAR;
	texParams.minFilter = GL_LINEAR;
	texParams.wrapS = GL_REPEAT;
	texParams.wrapT = GL_REPEAT;
	tex->setTexParameters(texParams);

	// shader
	auto glProgram = GLProgram::createWithFilenames(vertFile, fragFile);
	auto glProgramState = GLProgramState::getOrCreateWithGLProgram(glProgram);
	glProgramState->setUniformTexture("u_terrain", tex);
	glProgramState->setUniformVec2("u_texSize", Vec2(tex->getContentSize().width, tex->getContentSize().height));
	setGLProgramState(glProgramState);

	return true;
}

bool TerrainSprite::init(std::vector<Vec2>& points, Texture2D* tex, const std::string& vertFile, const std::string& fragFile)
{	
	do 
	{
		CC_BREAK_IF(!Node::init());
		CC_BREAK_IF(!initShader(tex, vertFile, fragFile));
		CC_BREAK_IF(!initTriangles(points));
		
		getGLProgramState()->setVertexAttribPointer("a_position", 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), &_triangles[0]);
		if (_bUserTexCoord)
		{
			getGLProgramState()->setVertexAttribPointer("a_texCoord", 2, GL_FLOAT, GL_FALSE, sizeof(Vec2), &_texCoords[0]);
		}

		return true;
	} while (0);

	return false;
}

void TerrainSprite::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
	_customCmd.init(_globalZOrder, transform, flags);
	_customCmd.func = CC_CALLBACK_0(TerrainSprite::onDraw, this, transform, flags);
	renderer->addCommand(&_customCmd);
}

void TerrainSprite::onDraw(const Mat4 &transform, uint32_t flags)
{
	getGLProgramState()->apply(transform);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, _triangles.size());
	CC_INCREMENT_GL_DRAWN_BATCHES_AND_VERTICES(1, _triangles.size());
}
