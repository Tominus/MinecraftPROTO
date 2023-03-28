#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"
#include "Chunk.h"
#include "World.h"
#include "Chunks_Manager.h"

Chunk_Data::Chunk_Data(Chunk* _ownerChunk)
{
	chunkManager = World::Instance().GetChunksManager();
	ownerChunk = _ownerChunk;
	
	blocks = nullptr;
	
	bHasFinishWaitSideChunk = false;

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
	WaitForSingleObject(chunkManager->mutex, INFINITE);
	const glm::vec3& _position = _chunk->GetChunkPosition();
	const glm::vec3& _ownerPosition = ownerChunk->chunkPosition;
	const glm::vec3& _diffPosition = _position - _ownerPosition;

	size_t _size = chunkPositionToWait.size();
	for (size_t i = 0; i < _size; ++i)
	{
		if (chunkPositionToWait[i] == _position)
		{
			if (_diffPosition.x == -1)
			{
				leftChunk = _chunk;
				leftChunk->chunkData->rightChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(leftChunk->chunkData, _ownerPosition);
				break;
			}
			if (_diffPosition.x == 1)
			{
				rightChunk = _chunk;
				rightChunk->chunkData->leftChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(rightChunk->chunkData, _ownerPosition);
				break;
			}

			if (_diffPosition.y == -1)
			{
				downChunk = _chunk;
				downChunk->chunkData->upChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(downChunk->chunkData, _ownerPosition);
				break;
			}
			if (_diffPosition.y == 1)
			{
				upChunk = _chunk;
				upChunk->chunkData->downChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(upChunk->chunkData, _ownerPosition);
				break;
			}

			if (_diffPosition.z == -1)
			{
				backChunk = _chunk;
				backChunk->chunkData->frontChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(backChunk->chunkData, _ownerPosition);
				break;
			}
			if (_diffPosition.z == 1)
			{
				frontChunk = _chunk;
				frontChunk->chunkData->backChunk = ownerChunk;
				--_size;

				AddOtherSideChunk(frontChunk->chunkData, _ownerPosition);
				break;
			}
		}
	}

	if (_size == 0)
	{
		bHasFinishWaitSideChunk = true;
		chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
	}

	ReleaseMutex(chunkManager->mutex);
}

void Chunk_Data::AddOtherSideChunk(Chunk_Data*& _otherChunkData, const glm::vec3& _ownerPosition)
{
	std::vector<glm::vec3>& _chunkPositionToWait = _otherChunkData->chunkPositionToWait;

	size_t _sizeWait = _chunkPositionToWait.size();
	for (size_t i = 0; i < _sizeWait; ++i)
	{
		if (_chunkPositionToWait[i] == _ownerPosition)
		{
			_chunkPositionToWait.erase(_chunkPositionToWait.begin() + i);
			--_sizeWait;
			break;
		}
	}

	if (_sizeWait == 0)
	{
		_otherChunkData->bHasFinishWaitSideChunk = true;
		chunkManager->onChunkInitialized.RemoveDynamic(_otherChunkData, &Chunk_Data::AddSideChunk);
	}
}

bool Chunk_Data::CheckChunkToWaitEmpty()
{
	if (!bHasFinishWaitSideChunk)return false;

	size_t _size = chunkPositionToWait.size();
	const glm::vec3& _ownerPosition = ownerChunk->chunkPosition;

	for (size_t i = 0; i < _size; ++i)
	{
		const glm::vec3& _position = chunkPositionToWait[i];
		const glm::vec3& _diffPosition = _position - _ownerPosition;

		if (_diffPosition.x == -1)
		{
			if (leftChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				leftChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.x == 1)
		{
			if (rightChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				rightChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}

		else if (_diffPosition.y == -1)
		{
			if (downChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				downChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.y == 1)
		{
			if (upChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				upChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}

		else if (_diffPosition.z == -1)
		{
			if (backChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				backChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.z == 1)
		{
			if (frontChunk)
			{
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->GetChunkAtPosition(_ownerPosition + _diffPosition))
			{
				frontChunk = _chunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
	}

	return _size == 0;
}