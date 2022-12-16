#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <chrono>
#include <ctime>

class TextureLoader;
class Chunks_Manager;
class Blocks_Global_Shapes;

class Thread_Obj;//

class World
{
	friend class MainGame;

private:
	World() {};
	~World();

	void InitWorld();

	void Start();
	void Update();
	void Tick();

	void Generate(const glm::vec3& _location, Thread_Obj* _thread);

public:
	inline static World& Instance()
	{
		static World instance;
		return instance;
	}

	inline TextureLoader* GetTextureLoader() const { return textureLoader; }
	inline Chunks_Manager* GetChunksManager() const { return chunksManager; }
	inline Blocks_Global_Shapes* GetBlocksGlobalShapes() const { return blocksGlobalShapes; }
	inline const GLuint& GetMatrixID() const { return matrixID; }

private:
	TextureLoader* textureLoader;
	Chunks_Manager* chunksManager;
	Blocks_Global_Shapes* blocksGlobalShapes;

	GLuint programID;
	GLuint matrixID;
	GLuint vertexArrayID;

	float gameTime;
	float tickTime;
	float fpsTime;
	float deltaTime;

	glm::vec3 pos;//

	//Init WorldClass
	//Load datas
	
	// In Game :
	//Awake
	//Start
	//Update
};