#include "World.h"

#include "GlobalDefine.h"
#include "TextureLoader.h"
#include "Chunks_Manager.h"
#include "Blocks_Global_Shapes.h"
#include "Thread_Manager.h"
#include "Thread_Obj.h"

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
	Thread_Manager* _threadManager = &Thread_Manager::Instance();

	for (size_t x = 0; x < 2; x++)
	{
		for (size_t y = 0; y < 2; y++)
		{
			for (size_t z = 0; z < 2; z++)
			{
				Thread_Obj* _thread = _threadManager->GetThreadObj();
				_thread->thread = std::thread(Generate);

				
			}
		}
	}
}

void World::Update()
{
	chunksManager->Render();
}

void World::Generate()
{
	chunksManager->AddChunk(glm::vec3(x, y, z));
	_thread->FinishThread();
}