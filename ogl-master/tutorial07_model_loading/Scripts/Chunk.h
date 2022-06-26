#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Chunk_Render;
class Chunk_Data;

class Chunk
{
	friend class ChunksManager;

private:
	Chunk(const GLuint& _matrixID, const glm::vec3& _position);
	~Chunk();

	void Render();

private:
	Chunk_Data* chunkData;
	Chunk_Render* chunkRender;

	glm::vec3 chunkPosition;
	glm::vec3 worldPosition;
};