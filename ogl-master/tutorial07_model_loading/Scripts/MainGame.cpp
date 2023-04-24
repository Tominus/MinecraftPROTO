#include "MainGame.h"
#include "World.h"
#include "Thread_Manager.h"
#include "Shaders_Manager.h"
#include "PerlinNoise.h"
#include <common/controls.hpp>

MainGame::MainGame()
{
	window = nullptr;
	threadManager = nullptr;
	world = nullptr;
}

MainGame::MainGame(GLFWwindow* _window)
{
	window = _window;
	threadManager = nullptr;
	world = nullptr;
}

MainGame::~MainGame()
{
	delete world;
	delete threadManager;
	delete Shaders_Manager::Instance();
	delete Perlin_Noise::Instance();
}

void MainGame::GameLoop()
{
	//--- Pre Init
	threadManager = Thread_Manager::Instance();
	//threadManager->Initialize();


	//--- Init
	world = World::Instance();
	world->InitWorld(window);


	//--- Start
	const GLuint& _matrixID = world->GetShadersManager()->GetMatrixID();
	world->Start();


	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs();
		const glm::mat4& MVP = getProjectionMatrix() * getViewMatrix() * glm::mat4(1.0f);
		glUniformMatrix4fv(_matrixID, 1, GL_FALSE, &MVP[0][0]);

		//--- Update
		world->Update();

		glfwSwapBuffers(window);
		glfwPollEvents();

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	//--- Exit
	world->Exit();

	//---Waiting Thread To Stop there behavior
	do {

	} while (threadManager->GetHasAllThreadFinished() == false);
}