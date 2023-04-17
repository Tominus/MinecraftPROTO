#pragma once
#include "GlobalDefine.h"

#include <Windows.h>

class Chunk_Data;
class Chunks_Manager;
class Perlin_Noise;
class Chunk_SideData;

class Chunk_Data_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Data_Generator() = delete;
	Chunk_Data_Generator(Chunks_Manager* _chunksManager);
	~Chunk_Data_Generator();

	/*Generate all existing block randomly.*/
	void GenerateNewChunkData(Chunk_Data*& _chunkData);
	void GenerateChunkSideData(const Perlin_Noise& _noiseCopy, Chunk* _chunk);
	Threaded void SetSideChunks(Chunk_Data*& _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	mutable HANDLE mutex_ChunkManager;
	HANDLE mutex_ChunkDataGenerator;

	unsigned randMax;
	Chunks_Manager* chunksManager;
};