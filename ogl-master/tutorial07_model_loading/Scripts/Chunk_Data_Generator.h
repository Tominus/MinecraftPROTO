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

	inline EBlock_Type GenerateHeight(const glm::vec3& _blockPosition)
	{
		//Hard coded values to smooth world
		double _noise = perlinNoise->CalculateNoise(_blockPosition.x / 2.f,	_blockPosition.z / 2.f);
		_noise /= 8.0;
		_noise += 128.0;

		if (_noise < _blockPosition.y)
		{
			return EBlock_Type::Air;
		}
		else
		{
			return static_cast<EBlock_Type>(rand() % 4 + 2);
		}
	}

	void SetSideChunks(Chunk_Data*& _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	mutable HANDLE mutex_ChunkManager;
	HANDLE mutex_ChunkDataGenerator;

	float fMinChunkHeight;
	float fMaxChunkHeight;
	unsigned randMax;
	Chunks_Manager* chunksManager;
	Chunk_Pool_Manager* chunkPoolManager;
	Perlin_Noise* perlinNoise;
};