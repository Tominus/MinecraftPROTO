#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Block_Render;

class Chunk
{
	friend class ChunksManager;

private:
	Chunk(const GLuint& _matrixID, const glm::vec3& _position);
	~Chunk();

	void Render();

private:
	std::vector<std::vector<std::vector<Block_Render*>>> blocks;
	// TODO Chunk_Data && Chunk_Render + Generator

	glm::vec3 chunkPosition;
	glm::vec3 worldPosition;
};