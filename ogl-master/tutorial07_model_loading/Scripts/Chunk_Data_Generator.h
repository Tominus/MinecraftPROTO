#pragma once
#include "GlobalDefine.h"

#include "PerlinNoise.h"
#include "Block_Type.h"
#include "Block.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Windows.h>

class Chunk_Data;
class Chunks_Manager;
class Chunk_SideData;
class Block;

class Chunk_Data_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Data_Generator() = delete;
	Chunk_Data_Generator(Chunks_Manager* _chunksManager);
	~Chunk_Data_Generator();

	/*Generate all existing block randomly.*/
	void GenerateNewChunkData(Chunk_Data* _chunkData);
	void GenerateChunkSideData(Chunk* _chunk);

	inline Block* GenerateBlock(const glm::vec3& _blockPosition)
	{
		//Smooth world
		double _noise = perlinNoise->CalculateNoise(_blockPosition.x / 2, _blockPosition.z / 2);
		_noise /= 8.0;
		_noise += 128.0;

		if (_noise < _blockPosition.y)
		{
			return new Block(EBlock_Type::Air);
		}
		else
		{
			return new Block((EBlock_Type)(rand() % 4 + 2)); //Generate only stone
		}
	}

	Threaded void SetSideChunks(Chunk_Data*& _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	mutable HANDLE mutex_ChunkManager;
	HANDLE mutex_ChunkDataGenerator;

	float fMinChunkHeight;
	float fMaxChunkHeight;
	unsigned randMax;
	Chunks_Manager* chunksManager;
	Perlin_Noise* perlinNoise;
};