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
	void InitDebug();
	void UpdateDebug();

	void Debug_DrawChunk();
	void Debug_DrawDebugScaledChunk();

private:
	GLFWwindow* window;
	GLuint matrixID_Debug;
	GLuint matrixID_Color_Debug;
	GLuint programID_Debug;

	bool bDebugDrawChunk;
	GLuint vertexsBufferDrawChunk;
	unsigned vertexsSizeDrawChunk;

	float fHalfChunkSize;
	float fChunkSize;
	float fDoubleChunkSize;

	float* debugColor;
};