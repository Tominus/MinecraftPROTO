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
	void Loop();
	void Tick();
	void Update();
	void Exit();
	void PostExitUpdate();

public:
	inline static World* Instance()
	{
		static World* _world = new World();
		return _world;
	}

	inline TextureLoader* GetTextureLoader() const { return textureLoader; }
	inline Chunks_Manager* GetChunksManager() const { return chunksManager; }
	inline Blocks_Global_Shapes* GetBlocksGlobalShapes() const { return blocksGlobalShapes; }
	inline Blocks_Global_Datas* GetBlocksGlobalDatas() const { return blocksGlobalDatas; }
	inline Perlin_Noise* GetPerlinNoise() const { return perlinNoise; }
	inline Shaders_Manager* GetShadersManager() const { return shaderManager; }

	inline GLFWwindow* GetWindow() const { return window; }

	inline const bool& GetIsExiting() const { return bIsExiting; }
	inline const float& GetDeltaTime() const { return deltaTime; }
	inline const float& GetGameBeginFrameTime() const { return gameTime; }
	inline const float& GetGameTime() const { return (float)std::clock() / 1000.f; }

	//Return the time elapsed in ms between last call
	inline const long& GetElapsedTime() const
	{
		const long& _currentClock = std::clock();
		const long& _elapsed = _currentClock - lastTime;
		lastTime = _currentClock;
		return _elapsed;
	}

private:
	TextureLoader* textureLoader;
	Chunks_Manager* chunksManager;
	Blocks_Global_Shapes* blocksGlobalShapes;
	Blocks_Global_Datas* blocksGlobalDatas;
	Debug_World* debugWorld;
	Perlin_Noise* perlinNoise;
	Shaders_Manager* shaderManager;

	GLFWwindow* window;

	bool bIsExiting;
	float deltaTime;
	float gameTime;
	float tickTime;
	float fpsTime;
	mutable long lastTime;
};