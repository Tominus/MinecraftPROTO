#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glfw3.h>

#include <chrono>
#include <ctime>

class TextureLoader;
class Chunks_Manager;
class Blocks_Global_Shapes;
class Blocks_Global_Datas;
class Debug_World;
class Perlin_Noise;
class Shaders_Manager;

class World
{
	friend class MainGame;

private:
	World();
	~World();

	void InitWorld(GLFWwindow* _window);

	void Start();
	void Update();
	void Tick();
	void Exit();

public:
	inline static World& Instance()
	{
		static World _world;
		return _world;
	}

	inline TextureLoader* GetTextureLoader() const { return textureLoader; }
	inline Chunks_Manager* GetChunksManager() const { return chunksManager; }
	inline Blocks_Global_Shapes* GetBlocksGlobalShapes() const { return blocksGlobalShapes; }
	inline Blocks_Global_Datas* GetBlocksGlobalDatas() const { return blocksGlobalDatas; }
	inline Perlin_Noise* GetPerlinNoise() const { return perlinNoise; }
	inline Shaders_Manager* GetShadersManager() const { return shaderManager; }

	inline const float& GetDeltaTime() const { return deltaTime; }
	inline const float& GetGameTime() const { return gameTime; }

private:
	TextureLoader* textureLoader;
	Chunks_Manager* chunksManager;
	Blocks_Global_Shapes* blocksGlobalShapes;
	Blocks_Global_Datas* blocksGlobalDatas;
	Debug_World* debugWorld;
	Perlin_Noise* perlinNoise;
	Shaders_Manager* shaderManager;

	float deltaTime;
	float gameTime;
	float tickTime;
	float fpsTime;
};