#include "Chunk_Data_Generator.h"

#include <vector>
#include <random>
#include <glm/detail/type_vec.hpp>

#include "GlobalDefine.h"
#include "Chunks_Manager.h"
#include "Chunk_Data.h"
#include "Chunk.h"
#include "Block.h"
#include "Block_Type.h"

Chunk_Data_Generator::Chunk_Data_Generator(Chunks_Manager* _chunksManager)
{
	chunksManager = _chunksManager;
	mutex = CreateMutex(0, false, 0);
	randMax = (unsigned)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{
	CloseHandle(mutex);
}

void Chunk_Data_Generator::GenerateNewChunkData(Chunk_Data*& _chunkData) const
{
	//SetSideChunks(_chunkData);

	std::random_device _rd;
	std::mt19937 _gen(_rd());
	std::uniform_int_distribution<> _dist(0, randMax - 1);

	Block****& _blocks = _chunkData->blocks;
	_blocks = new Block***[Chunk_Size];

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block***& _blocksX = _blocks[x];
		_blocksX = new Block**[Chunk_Size];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Block**& _blocksXY = _blocksX[y];
			_blocksXY = new Block*[Chunk_Size];

			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				const EBlock_Type& _type = (EBlock_Type)(_dist(_gen));
				_blocksXY[z] = new Block(_type);
			}
		}
	}
}

void Chunk_Data_Generator::SetSideChunks(Chunk_Data*& _chunkData) const
{
	WaitForSingleObject(mutex, INFINITE);

	Chunk*& _ownerChunk = _chunkData->ownerChunk;
	const glm::vec3& _ownerChunkPosition = _ownerChunk->GetChunkPosition();

	if (chunksManager->worldChunks.size() > 9)
	{
		printf("aaaaaa");
	}
	
	if (Chunk* _downChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(0, -1, 0)))
	{
		_downChunk->chunkData->upChunk = _ownerChunk;
		_chunkData->downChunk = _downChunk;
	}
	if (Chunk* _upChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(0, 1, 0)))
	{
		_upChunk->chunkData->downChunk = _ownerChunk;
		_chunkData->upChunk = _upChunk;
	}

	if (Chunk* _leftChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(-1, 0, 0)))
	{
		_leftChunk->chunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
	}
	if (Chunk* _rightChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(1, 0, 0)))
	{
		_rightChunk->chunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
	}

	if (Chunk* _backChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(0, 0, -1)))
	{
		_backChunk->chunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
	}
	if (Chunk* _frontChunk = chunksManager->GetChunkAtPosition(_ownerChunkPosition + glm::vec3(0, 0, 1)))
	{
		_frontChunk->chunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
	}

	ReleaseMutex(mutex);
}