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

	window = nullptr;

	bIsExiting = false;
	gameTime = 0.f;
	tickTime = 0.f;
	fpsTime = 0.f;
	deltaTime = 0.f;
	lastTime = 0;
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
	window = _window;

	shaderManager = Shaders_Manager::Instance();
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

	perlinNoise = Perlin_Noise::Instance();
}

void World::Start()
{
	perlinNoise->InitializeSeed(69);
	perlinNoise->GenerateImage();

	chunksManager->StartChunkManager();
	chunksManager->AddStartingWorldBaseChunk();
}

void World::Loop()
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
		Update();
	}
}

void World::Tick()
{
	chunksManager->TickChunksManager();
}

void World::Update()
{
	chunksManager->UpdateChunksManager();
	debugWorld->UpdateDebug();
}

void World::Exit()
{
	bIsExiting = true;
	chunksManager->Exit();
}

void World::PostExitUpdate()
{
	chunksManager->PostExitUpdate();
}