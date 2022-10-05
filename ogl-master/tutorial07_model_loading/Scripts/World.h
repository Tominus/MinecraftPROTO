#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class TextureLoader;
class Chunks_Manager;
class Blocks_Global_Shapes;

class World
{
	friend class MainGame;

private:
	World() {};
	~World();

	void InitWorld();

	void Start();
	void Update();

	void Generate();

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

	//Init WorldClass
	//Load datas
	
	// In Game :
	//Awake
	//Start
	//Update

};