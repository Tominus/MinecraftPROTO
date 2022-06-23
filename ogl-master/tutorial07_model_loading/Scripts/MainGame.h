#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glfw3.h>

class MainGame
{
public:
	MainGame();
	MainGame(GLFWwindow* _window);
	~MainGame();

	void GameLoop();

private:
	GLFWwindow* window; //TODO Window class
};