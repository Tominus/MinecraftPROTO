#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


class Block_Render
{
public:
	Block_Render(GLuint _textureID, GLuint _matrixID);
	~Block_Render();

	void Render(const glm::vec3& _chunkOffset);


private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;

	GLuint textureID;
	GLuint matrixID;

	GLuint vertexbuffer;
	GLuint uvbuffer;
};