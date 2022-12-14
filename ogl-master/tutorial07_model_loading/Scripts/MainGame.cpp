#include "MainGame.h"
#include "World.h"
#include "Thread_Manager.h"
#include <common/controls.hpp>

MainGame::MainGame()
{
	window = nullptr;
	threadManager = nullptr;
}

MainGame::MainGame(GLFWwindow* _window)
{
	window = _window;
	threadManager = nullptr;
}

MainGame::~MainGame()
{

}

void MainGame::GameLoop()
{
	//--- Pre Init
	threadManager = &Thread_Manager::Instance();
	threadManager->Initialize();


	//--- Init
	World* _world = &World::Instance();
	_world->InitWorld();


	//--- Start
	const GLuint& _matrixID = _world->GetMatrixID();
	_world->Start();


	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs();
		const glm::mat4& MVP = getProjectionMatrix() * getViewMatrix() * glm::mat4(1.0);
		glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &MVP[0][0]);

		//--- Update
		_world->Update();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//---Exit
	do {

	} while (threadManager->GetHasAllThreadFinished() == false);
}