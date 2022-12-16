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
	//Waiting detach every thread before deleting others
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
	
}

void World::Update()
{
	const float& _clock = std::clock() / 1000.0f;
	
	deltaTime = _clock - gameTime;
	tickTime += deltaTime;
	fpsTime += deltaTime;
	gameTime = _clock;

	if (tickTime > Tick_Time)
	{
		tickTime -= Tick_Time;
		Tick();
	}
	if (fpsTime > Fps_Time)
	{
		fpsTime -= 0.f;
		chunksManager->UpdateChunksManager();
	}
}

void World::Tick()
{
	chunksManager->TickChunksManager();
}

void World::Generate(const glm::vec3& _location, Thread_Obj* _thread)
{
	chunksManager->AddChunk(_location);
}