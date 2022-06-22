#pragma once
#include <vector>

#include "Block_Render.h"

class Chunk
{
public:
	Chunk(GLuint _textureID, GLuint _matrixID);
	~Chunk();

	void Render();

private:
	std::vector<std::vector<std::vector<Block_Render*>>> blocks;
};