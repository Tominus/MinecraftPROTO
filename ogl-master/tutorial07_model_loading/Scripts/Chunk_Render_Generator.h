#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>

class Chunk_Data;
class Chunk_Render;

class World;
class TextureLoader;
class Blocks_Global_Shapes;

struct SChunk_Render_Data;

class Chunk_Render_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Render_Generator() = delete;
	Chunk_Render_Generator(Chunks_Manager* _chunksManager);
	~Chunk_Render_Generator();

	void GenerateNewChunkRender(Chunk_Render* _chunkRender, Chunk_Data* _chunkData);
	void GenerateChunkCGRender(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas);

	SChunk_Render_Data* GetChunkRenderData(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, const GLuint& _textureID);

private:
	Chunks_Manager* chunksManager;

	GLuint matrixID;

	World* world;
	TextureLoader* textureLoader;
	Blocks_Global_Shapes* blocksGlobalShapes;
};