#include "World.h"

#include "GlobalDefine.h"
#include "TextureLoader.h"
#include "Chunks_Manager.h"
#include "Debug_World.h"
#include "Blocks_Global_Shapes.h"
#include "Blocks_Global_Datas.h"

#include <common/shader.hpp>

World::World()
{
	textureLoader = nullptr;
	chunksManager = nullptr;
	blocksGlobalShapes = nullptr;
	debugWorld = nullptr;
	blocksGlobalDatas = nullptr;

	programID = 0;
	matrixID = 0;
	vertexArrayID = 0;

	gameTime = 0.f;
	tickTime = 0.f;
	fpsTime = 0.f;
	deltaTime = 0.f;
}

World::~World()
{
	delete debugWorld;
	delete chunksManager;
	delete textureLoader;
	delete blocksGlobalShapes;
	delete blocksGlobalDatas;

	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayID);
}

void World::InitWorld(GLFWwindow* _window)
{
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	matrixID = glGetUniformLocation(programID, "MVP");
	glUseProgram(programID);

	debugWorld = new Debug_World(_window);
	debugWorld->InitDebug(matrixID);

	textureLoader = new TextureLoader();
	textureLoader->LoadTextures();

	blocksGlobalShapes = new Blocks_Global_Shapes();
	blocksGlobalShapes->GenerateBlocksShapeDatas();

	blocksGlobalDatas = new Blocks_Global_Datas();
	blocksGlobalDatas->GenerateBlocksGlobalDatas();

	chunksManager = new Chunks_Manager();
}

void World::Start()
{
	chunksManager->StartChunkManager();
	chunksManager->AddStartingWorldBaseChunk();
}

void World::Update()
{
	const float& _clock = std::clock() / 1000.f;
	
	deltaTime = _clock - gameTime;
	tickTime += deltaTime;
	fpsTime += deltaTime;
	gameTime = _clock;

	if (tickTime > Tick_Time)
	{
		tickTime -= Tick_Time;
		Tick();
	}
	//if (fpsTime > Fps_Time)
	{
	//	fpsTime -= Fps_Time;
		chunksManager->UpdateChunksManager();
		debugWorld->UpdateDebug();
	}
}

void World::Tick()
{
	chunksManager->TickChunksManager();
}

void World::Exit()
{
	chunksManager->Exit();
}