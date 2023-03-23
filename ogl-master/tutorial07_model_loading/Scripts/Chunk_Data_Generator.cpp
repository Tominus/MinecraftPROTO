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
#include "Action.h"

Chunk_Data_Generator::Chunk_Data_Generator(Chunks_Manager* _chunksManager)
{
	chunksManager = _chunksManager;
	mutex_ChunkManager = _chunksManager->mutex;
	randMax = (unsigned)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{
	
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
				int _i = (_dist(_gen));
				if (_i == 6 || _i == 7 || _i == 8 || _i == 9)
					_i = 0;

				const EBlock_Type& _type = (EBlock_Type)(_i);
				_blocksXY[z] = new Block(_type);
			}
		}
	}
}

Threaded void Chunk_Data_Generator::SetSideChunks(Chunk_Data*& _chunkData) const
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);

	Chunk*& _ownerChunk = _chunkData->ownerChunk;
	const glm::vec3& _ownerChunkPosition = _ownerChunk->GetChunkPosition();

	const glm::vec3& _downPosition = _ownerChunkPosition + glm::vec3(0, -1, 0);
	const glm::vec3& _upPosition = _ownerChunkPosition + glm::vec3(0, 1, 0);
	const glm::vec3& _leftPosition = _ownerChunkPosition + glm::vec3(-1, 0, 0);
	const glm::vec3& _rightPosition = _ownerChunkPosition + glm::vec3(1, 0, 0);
	const glm::vec3& _backPosition = _ownerChunkPosition + glm::vec3(0, 0, -1);
	const glm::vec3& _frontPosition = _ownerChunkPosition + glm::vec3(0, 0, 1);

	
	if (Chunk* _downChunk = chunksManager->GetChunkAtPosition(_downPosition))
	{
		_downChunk->chunkData->upChunk = _ownerChunk;
		_chunkData->downChunk = _downChunk;
	}
	if (Chunk* _upChunk = chunksManager->GetChunkAtPosition(_upPosition))
	{
		_upChunk->chunkData->downChunk = _ownerChunk;
		_chunkData->upChunk = _upChunk;
	}

	if (Chunk* _leftChunk = chunksManager->GetChunkAtPosition(_leftPosition))
	{
		_leftChunk->chunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
	}
	else if(Chunk * _leftChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_leftPosition))
	{
		_leftChunk->chunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_leftPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_leftPosition);
	}

	if (Chunk* _rightChunk = chunksManager->GetChunkAtPosition(_rightPosition))
	{
		_rightChunk->chunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
	}
	else if (Chunk* _rightChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_rightPosition))
	{
		_rightChunk->chunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
	}
	else
		_chunkData->chunkPositionToWait.push_back(_rightPosition);

	if (Chunk* _backChunk = chunksManager->GetChunkAtPosition(_backPosition))
	{
		_backChunk->chunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
	}
	else if (Chunk* _backChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_backPosition))
	{
		_backChunk->chunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
	}
	else
		_chunkData->chunkPositionToWait.push_back(_backPosition);

	if (Chunk* _frontChunk = chunksManager->GetChunkAtPosition(_frontPosition))
	{
		_frontChunk->chunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
	}
	else if (Chunk* _frontChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_frontPosition))
	{
		_frontChunk->chunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
	}
	else
		_chunkData->chunkPositionToWait.push_back(_frontPosition);

	chunksManager->onChunkInitialized.AddDynamic(_chunkData, &Chunk_Data::AddSideChunk);

	ReleaseMutex(mutex_ChunkManager);
}