#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


class Block_Render
{
	friend class Chunk; //debug for alpha

public:
	Block_Render(GLuint _textureID, GLuint _matrixID);
	~Block_Render();

	void Render(const glm::vec3& _chunkOffset);
	void Render_Alpha(const glm::vec3& _chunkOffset);

	void Render(); //TODO delete
	void Render_Alpha();


private:
	unsigned int verticesSize;

	glm::vec3 position;

	GLuint textureID;
	GLuint matrixID;

	GLuint vertexbuffer;
	GLuint uvbuffer;
};