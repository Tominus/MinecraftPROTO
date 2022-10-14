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
	//Detach every thread before deleting others
	delete chunksManager;

	delete textureLoader;
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

	for (size_t x = 0; x < 1; x++)
	{
		for (size_t y = 0; y < 1; y++)
		{
			for (size_t z = 0; z < 1; z++)
			{
				Thread_Obj* _thread = _threadManager->GetValidThreadObj();
				if (_thread)
					_thread->TEST(chunksManager, glm::vec3(x, y, z));
			}
		}
	}
}

void World::Update()
{
	chunksManager->UpdateChunksManager();

	Thread_Manager* _threadManager = &Thread_Manager::Instance();
	Thread_Obj* _thread = _threadManager->GetValidThreadObj();
	if (_thread)
	{
		if (pos.x < pos.z)
		{
			++pos.x;
			pos.z = 0;
		}
		else
		{
			++pos.z;
		}
		_thread->TEST(chunksManager, pos);
		/*_thread->AddThread([&, this](const glm::vec3& _location, Thread_Obj* _thread)
			{
				Generate(_location, _thread);
			}, pos, _thread);*/
		/*_thread->AddThread_OneParam(
			[&, this](int _uwu)
			{
				ToDelete();
			}, 5);*/
	}
}

void World::Generate(const glm::vec3& _location, Thread_Obj* _thread)
{
	chunksManager->AddChunk(_location);
}