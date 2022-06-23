#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class TextureLoader;
class ChunksManager;

class World
{
	friend class MainGame;

private:
	World() {};
	~World();

	void InitWorld();

	void Start();
	void Update();

public:
	inline static World& Instance()
	{
		static World instance;
		return instance;
	}

	inline TextureLoader* GetTextureLoader() const { return textureLoader; }
	inline ChunksManager* GetChunkManager() const { return chunkManager; }
	inline const GLuint& GetMatrixID() const { return matrixID; }

private:
	TextureLoader* textureLoader;
	ChunksManager* chunkManager;

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