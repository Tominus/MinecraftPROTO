#pragma once
#include <GL/glew.h>
#include <map>

#include "GlobalDefine.h"
#include "lodepng.h"

#include "Block_Type.h"

class TextureLoader
{
	friend class World;
	friend class Chunk_Render_Generator;//

private:
	TextureLoader();
	~TextureLoader();

	void LoadTextures();

	GLuint LoadFile_PNG(const char* _path);

public:
	const GLuint& GetBlockTextureID(const EBlock_Type& _blockType) const;

private:
	std::map<EBlock_Type, GLuint> blockTextureID;
};