#pragma once
#include "GlobalDefine.h"

#include <Windows.h>

class Chunk_Data;
class Chunks_Manager;

class Chunk_Data_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Data_Generator() = delete;
	Chunk_Data_Generator(Chunks_Manager* _chunksManager);
	~Chunk_Data_Generator();

	/*Generate all existing block randomly.*/
	void GenerateNewChunkData(Chunk_Data*& _chunkData) const;
	Threaded void SetSideChunks(Chunk_Data*& _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	mutable HANDLE mutex_ChunkManager;

	unsigned randMax;
	Chunks_Manager* chunksManager;
};