#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glfw3.h>

class Debug_World
{

	friend class World;

private:
	Debug_World(GLFWwindow* _window);
	~Debug_World();

private:
	void InitDebug(const GLuint& _matrixID);
	void UpdateDebug();

	void Debug_DrawChunk();
	void Debug_DrawDebugScaledChunk();

private:
	GLFWwindow* window;
	GLuint matrixID;

	bool bDebugDrawChunk;
	GLuint vertexsBufferDrawChunk;
	unsigned vertexsSizeDrawChunk;
};