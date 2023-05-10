#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <Windows.h>

#include "Chunk_Render.h"
#include "Chunk_Pool_Manager.h"

class Chunk_Data;
class Chunk_Render;

class World;
class TextureLoader;
class Blocks_Global_Datas;
class Blocks_Global_Shapes;

class Chunk_Render_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Render_Generator() = delete;
	Chunk_Render_Generator(Chunks_Manager* _chunksManager);
	~Chunk_Render_Generator();

	void GenerateNewChunkRender(Chunk_Render* _chunkRender, Chunk_Data* _chunkData);
	SChunk_Render_Data* GetChunkRenderData(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, const GLuint& _textureID);

	void GenerateChunkCGRender(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas);

	void AddTextureToUpdate(std::vector<GLuint>& _texturesToUpdate, const GLuint& _textureID);
	void RegenerateRender(Chunk* _chunk, std::vector<GLuint>& _texturesToUpdate);

	void GenerateChunkUpdatedCGRender(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, std::vector<GLuint>& _texturesToUpdate);
	void ClearUnUsedChunkCGRender();

private:
	Chunks_Manager* chunksManager;
	Chunk_Pool_Manager* chunkPoolManager;

	GLuint matrixID;

	HANDLE mutex_ChunkManager;

	float fMinHeightChunk;
	float fMaxHeightChunk;

	World* world;
	TextureLoader* textureLoader;
	Blocks_Global_Datas* blocksGlobalDatas;
	Blocks_Global_Shapes* blockGlobalShapes;
};