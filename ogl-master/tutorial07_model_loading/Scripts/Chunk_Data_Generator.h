#pragma once
#include "GlobalDefine.h"

#include "Block_Type.h"
#include "Block.h"

#include <Windows.h>

class Chunk_Data;
class Chunks_Manager;
class Perlin_Noise;
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
	void GenerateNewChunkData(Chunk_Data*& _chunkData);
	void GenerateChunkSideData(const Perlin_Noise& _noiseCopy, Chunk* _chunk);

	inline Block* GenerateBlock(float _noise, float _blockHeight)
	{
		//Smooth world

		_noise /= 8.f;
		_noise += 128.f;

		if (_noise < _blockHeight)
		{
			return new Block(EBlock_Type::Air);
		}
		else
		{
			return new Block((EBlock_Type)(rand() % 4 + 2)); //Generate only stone
		}
	}

	/*if (_noiseHeight < _blockHeight)
			{
				return new Block(EBlock_Type::Air);
			}
			else
			{
				return new Block((EBlock_Type)(rand() % 4 + 2));
			}*/

	Threaded void SetSideChunks(Chunk_Data*& _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	mutable HANDLE mutex_ChunkManager;
	HANDLE mutex_ChunkDataGenerator;

	float fMinChunkHeight;
	float fMaxChunkHeight;
	unsigned randMax;
	Chunks_Manager* chunksManager;
};