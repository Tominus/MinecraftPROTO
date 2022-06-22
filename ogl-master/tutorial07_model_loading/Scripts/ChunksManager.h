#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Chunk;

class ChunksManager
{
	friend class World;

private:
	ChunksManager();
	~ChunksManager();

	void AddChunk(const glm::vec3& _position);
	void Render();

private:
	std::vector<Chunk*> worldChunks;
	glm::vec3 lastChunkPosition;
};