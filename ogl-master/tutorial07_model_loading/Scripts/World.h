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

public:
	inline static World& Instance()
	{
		static World instance;
		return instance;
	}

	inline TextureLoader* GetTextureLoader() const { return textureLoader; }
	inline Chunks_Manager* GetChunksManager() const { return chunksManager; }
	inline Blocks_Global_Shapes* GetBlocksGlobalShapes() const { return blocksGlobalShapes; }
	inline Blocks_Global_Datas* GetBlocksGlobalDatas() const { return blocksGlobalDatas; }
	inline const GLuint& GetMatrixID() const { return matrixID; }

private:
	TextureLoader* textureLoader;
	Chunks_Manager* chunksManager;
	Blocks_Global_Shapes* blocksGlobalShapes;
	Blocks_Global_Datas* blocksGlobalDatas;
	Debug_World* debugWorld;

	GLuint programID;
	GLuint matrixID;
	GLuint vertexArrayID;

	float gameTime;
	float tickTime;
	float fpsTime;
	float deltaTime;
};