#pragma once
#include <GL/glew.h>
#include <map>

#include "GlobalDefine.h"
#include "lodepng.h"

#include "Block_Type.h"

class TextureLoader
{
	friend class World;

private:
	TextureLoader();
	~TextureLoader();

	void LoadTextures();

	/*Deprecated.*/
	GLuint LoadFile_DDS(const char* _path);
	/*Deprecated.*/
	GLuint LoadFile_BMP(const char* _path);
	GLuint LoadFile_PNG(const char* _path);

public:
	const GLuint& GetBlockTextureID(const Block_Type& _blockType) const;

private:
	std::map<Block_Type, GLuint> blockTextureID;
};