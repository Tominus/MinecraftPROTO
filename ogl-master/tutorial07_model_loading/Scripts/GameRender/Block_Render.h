#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>


class Block_Render
{
public:
	Block_Render(GLuint _textureID, GLuint _matrixID);
	~Block_Render();

	void Render();


private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;

	glm::vec3 position;

	GLuint textureID;
	GLuint matrixID;

	GLuint vertexbuffer;
	GLuint uvbuffer;
};