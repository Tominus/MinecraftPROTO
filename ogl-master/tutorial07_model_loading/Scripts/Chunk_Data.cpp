#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"
#include "Chunk.h"
#include "World.h"
#include "Chunks_Manager.h"

Chunk_Data::Chunk_Data(Chunk* _ownerChunk)
{
	ownerChunk = _ownerChunk;
	blocks = nullptr;

	bHasFinishWait = false;

	downChunk = nullptr;
	upChunk = nullptr;
	leftChunk = nullptr;
	rightChunk = nullptr;
	backChunk = nullptr;
	frontChunk = nullptr;
}

Chunk_Data::~Chunk_Data()
{
	if (downChunk)
		downChunk->chunkData->upChunk = nullptr;
	if (upChunk)
		upChunk->chunkData->downChunk = nullptr;
	if (leftChunk)
  		leftChunk->chunkData->rightChunk = nullptr;
	if (rightChunk)
 		rightChunk->chunkData->leftChunk = nullptr;
	if (backChunk)
		backChunk->chunkData->frontChunk = nullptr;
	if (frontChunk)
		frontChunk->chunkData->backChunk = nullptr;

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block*** _x = blocks[x];
		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Block** _y = _x[y];
			for (size_t z = 0; z < Chunk_Size; ++z)
				delete _y[z];
			delete[] _y;
		}
		delete[] _x;
	}
	delete[] blocks;
}

void Chunk_Data::AddSideChunk(Chunk* _chunk)
{
	Chunks_Manager* _chunkManager = World::Instance().GetChunksManager();

	WaitForSingleObject(_chunkManager->mutex, INFINITE);
	const glm::vec3& _position = _chunk->GetChunkPosition();

	size_t _size = chunkPositionToWait.size();
	for (size_t i = 0; i < _size; ++i)
	{
		if (chunkPositionToWait[i] == _position)
		{
			const glm::vec3& _diffPosition = _position - ownerChunk->chunkPosition;

			if (_diffPosition.x == -1)
			{
				leftChunk = _chunk;
				leftChunk->chunkData->rightChunk = ownerChunk;
				--_size;
				break;
			}
			if (_diffPosition.x == 1)
			{
				rightChunk = _chunk;
				rightChunk->chunkData->leftChunk = ownerChunk;
				--_size;
				break;
			}

			if (_diffPosition.y == -1)
			{
				downChunk = _chunk;
				downChunk->chunkData->upChunk = ownerChunk;
				--_size;
				break;
			}
			if (_diffPosition.y == 1)
			{
				upChunk = _chunk;
				upChunk->chunkData->downChunk = ownerChunk;
				--_size;
				break;
			}

			if (_diffPosition.z == -1)
			{
				backChunk = _chunk;
				backChunk->chunkData->frontChunk = ownerChunk;
				--_size;
				break;
			}
			if (_diffPosition.z == 1)
			{
				frontChunk = _chunk;
				frontChunk->chunkData->backChunk = ownerChunk;
				--_size;
				break;
			}
		}
	}

	if (_size == 0)
	{
		bHasFinishWait = true;
		_chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
		onAllSideValid.Invoke();
		onAllSideValid.RemoveDynamic(ownerChunk, &Chunk::InitChunkRender);
	}

	ReleaseMutex(_chunkManager->mutex);
}