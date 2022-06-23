#include "World.h"

#include "GlobalDefine.h"
#include "TextureLoader.h"
#include "ChunksManager.h"

#include <common/shader.hpp>

World::~World()
{
	delete textureLoader;
	delete chunkManager;
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

void World::InitWorld()
{
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	matrixID = glGetUniformLocation(programID, "MVP");
	glUseProgram(programID);

	textureLoader = new TextureLoader();
	textureLoader->LoadTextures();

	chunkManager = new ChunksManager();
}

void World::Start()
{
	for (size_t x = 0; x < 2; x++)
	{
		for (size_t y = 0; y < 2; y++)
		{
			for (size_t z = 0; z < 2; z++)
			{
				chunkManager->AddChunk(glm::vec3(x, y, z));
			}
		}
	}
}

void World::Update()
{
	chunkManager->Render();
}