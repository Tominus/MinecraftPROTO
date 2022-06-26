#include "Chunk.h"

#include "GlobalDefine.h"
#include "World.h"
#include "Chunk_Data.h"
#include "Chunk_render.h"

Chunk::Chunk(const GLuint& _matrixID, const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * 16.0f;
	chunkData = new Chunk_Data();
	chunkRender = new Chunk_Render();
}

Chunk::~Chunk()
{
	delete chunkData;
	delete chunkRender;
}

void Chunk::Render()
{

}