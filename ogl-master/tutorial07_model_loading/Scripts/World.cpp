#include "World.h"

#include "GlobalDefine.h"
#include "TextureLoader.h"
#include "Chunks_Manager.h"
#include "Debug_World.h"
#include "Blocks_Global_Shapes.h"
#include "Blocks_Global_Datas.h"
#include "PerlinNoise.h"
#include "Shaders_Manager.h"

World::World()
{
	textureLoader = nullptr;
	chunksManager = nullptr;
	blocksGlobalShapes = nullptr;
	blocksGlobalDatas = nullptr;
	debugWorld = nullptr;
	perlinNoise = nullptr;
	shaderManager = nullptr;

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
}

void World::InitWorld(GLFWwindow* _window)
{
	shaderManager = &Shaders_Manager::Instance();
	shaderManager->Initialize();

	debugWorld = new Debug_World(_window);
	debugWorld->InitDebug();

	textureLoader = new TextureLoader();
	textureLoader->LoadTextures();

	blocksGlobalShapes = new Blocks_Global_Shapes();
	blocksGlobalShapes->GenerateBlocksShapeDatas();

	blocksGlobalDatas = new Blocks_Global_Datas();
	blocksGlobalDatas->GenerateBlocksGlobalDatas();

	chunksManager = new Chunks_Manager();

	perlinNoise = &Perlin_Noise::Instance();
}

void World::Start()
{
	perlinNoise->Initialize();
	perlinNoise->SetSeed(69);

	perlinNoise->GenerateImage();

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