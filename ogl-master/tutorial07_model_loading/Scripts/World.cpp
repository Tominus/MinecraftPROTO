#include "World.h"

#include "GlobalDefine.h"
#include "TextureLoader.h"
#include "Chunks_Manager.h"
#include "Blocks_Global_Shapes.h"

#include <common/shader.hpp>

World::~World()
{
	delete textureLoader;
	delete chunksManager;
	delete blocksGlobalShapes;
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

	blocksGlobalShapes = new Blocks_Global_Shapes();
	blocksGlobalShapes->GenerateBlocksShapeDatas();

	chunksManager = new Chunks_Manager();
}

void World::Start()
{
	for (size_t x = 0; x < 1; x++)
	{
		for (size_t y = 0; y < 1; y++)
		{
			for (size_t z = 0; z < 1; z++)
			{
				chunksManager->AddChunk(glm::vec3(x, y, z));
			}
		}
	}
}

void World::Update()
{
	chunksManager->Render();
}