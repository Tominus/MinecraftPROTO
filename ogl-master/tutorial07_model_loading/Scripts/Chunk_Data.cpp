#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"
#include "Chunk.h"
#include "World.h"
#include "Chunks_Manager.h"

Chunk_Data::Chunk_Data(Chunk* _ownerChunk)
{
	chunkManager = World::Instance()->GetChunksManager();
	mutex_ChunkManager = chunkManager->mutex;
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
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
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
		chunkManager->onChunkDestroyed.RemoveDynamic(this, &Chunk_Data::RemoveSideChunk);
	}

	ReleaseMutex(mutex_ChunkManager);
}

Threaded void Chunk_Data::AddOtherSideChunk(Chunk_Data*& _otherChunkData, const glm::vec3& _ownerPosition)
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
		chunkManager->onChunkDestroyed.RemoveDynamic(_otherChunkData, &Chunk_Data::RemoveSideChunk);
	}
}

void Chunk_Data::AddGeneratedSideChunk(const glm::vec3& _otherPosition)
{
	size_t _sizeWait = chunkPositionToWait.size();
	for (size_t i = 0; i < _sizeWait; ++i)
	{
		if (chunkPositionToWait[i] == _otherPosition)
		{
			chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
			--_sizeWait;
			break;
		}
	}

	if (_sizeWait == 0)
	{
		bHasFinishWaitSideChunk = true;
		chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
		chunkManager->onChunkDestroyed.RemoveDynamic(this, &Chunk_Data::RemoveSideChunk);
	}
}

void Chunk_Data::RemoveSideChunk(Chunk* _chunk)
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
	const glm::vec3& _position = _chunk->GetChunkPosition();

	size_t _size = chunkPositionToWait.size();
	for (size_t i = 0; i < _size; ++i)
	{
		if (chunkPositionToWait[i] == _position)
		{
			chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
			--_size;
			break;
		}
	}

	if (_size == 0)
	{
		bHasFinishWaitSideChunk = true;
		chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
		chunkManager->onChunkDestroyed.RemoveDynamic(this, &Chunk_Data::RemoveSideChunk);
	}
	ReleaseMutex(mutex_ChunkManager);
}

bool Chunk_Data::CheckChunkToWaitEmpty()
{
	if (!bHasFinishWaitSideChunk)return false;

	WaitForSingleObject(mutex_ChunkManager, INFINITE);
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
				leftChunk->chunkData->rightChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				leftChunk = _chunk;
				_chunk->chunkData->rightChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.x == 1)
		{
			if (rightChunk)
			{
				rightChunk->chunkData->leftChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				rightChunk = _chunk;
				_chunk->chunkData->leftChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}

		else if (_diffPosition.y == -1)
		{
			if (downChunk)
			{
				downChunk->chunkData->upChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				downChunk = _chunk;
				_chunk->chunkData->upChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.y == 1)
		{
			if (upChunk)
			{
				upChunk->chunkData->downChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				upChunk = _chunk;
				_chunk->chunkData->downChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}

		else if (_diffPosition.z == -1)
		{
			if (backChunk)
			{
				backChunk->chunkData->frontChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				backChunk = _chunk;
				_chunk->chunkData->frontChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
		else if (_diffPosition.z == 1)
		{
			if (frontChunk)
			{
				frontChunk->chunkData->backChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
			else if (Chunk* _chunk = chunkManager->Opti_GetChunk(_ownerPosition + _diffPosition))
			{
				frontChunk = _chunk;
				_chunk->chunkData->backChunk = ownerChunk;
				chunkPositionToWait.erase(chunkPositionToWait.begin() + i);
				--_size;
				--i;
			}
		}
	}

	ReleaseMutex(mutex_ChunkManager);

	if (_size == 0)
	{
		chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
		chunkManager->onChunkDestroyed.RemoveDynamic(this, &Chunk_Data::RemoveSideChunk);
		return true;
	}
	return false;
}

void Chunk_Data::PreDelete()
{
	if (downChunk)
	{
		downChunk->chunkData->upChunk = nullptr;
		downChunk = nullptr;
	}
	if (upChunk)
	{
		upChunk->chunkData->downChunk = nullptr;
		upChunk = nullptr;
	}
	if (leftChunk)
	{
		leftChunk->chunkData->rightChunk = nullptr;
		leftChunk = nullptr;
	}
	if (rightChunk)
	{
		rightChunk->chunkData->leftChunk = nullptr;
		rightChunk = nullptr;
	}
	if (backChunk)
	{
		backChunk->chunkData->frontChunk = nullptr;
		backChunk = nullptr;
	}
	if (frontChunk)
	{
		frontChunk->chunkData->backChunk = nullptr;
		frontChunk = nullptr;
	}

	chunkManager->onChunkInitialized.RemoveDynamic(this, &Chunk_Data::AddSideChunk);
	chunkManager->onChunkDestroyed.RemoveDynamic(this, &Chunk_Data::RemoveSideChunk);
}