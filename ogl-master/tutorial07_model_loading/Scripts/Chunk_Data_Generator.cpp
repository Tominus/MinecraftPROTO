#include "Chunk_Data_Generator.h"

#include <vector>
#include <glm/detail/type_vec.hpp>

#include "Chunks_Manager.h"
#include "Chunk_SideData.h"
#include "Chunk_Data.h"
#include "Chunk.h"
#include "Block.h"
#include "Block_Type.h"
#include "Action.h"
#include "PerlinNoise.h"

Chunk_Data_Generator::Chunk_Data_Generator(Chunks_Manager* _chunksManager)
{
	chunksManager = _chunksManager;
	mutex_ChunkManager = _chunksManager->mutex;
	mutex_ChunkDataGenerator = CreateMutex(0, false, 0);
	randMax = (unsigned)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{
	ReleaseMutex(mutex_ChunkDataGenerator);
}

void Chunk_Data_Generator::GenerateNewChunkData(Chunk_Data*& _chunkData)
{
	WaitForSingleObject(mutex_ChunkDataGenerator, INFINITE);
	const Perlin_Noise _noiseCopy = Perlin_Noise::Instance();
	ReleaseMutex(mutex_ChunkDataGenerator);

	Chunk* _ownerChunk = _chunkData->ownerChunk;
	const glm::vec3& _chunkWorldPosition = _ownerChunk->worldPosition;

	if (/*Exist in file*/false)
	{
		// Load Data
	}
	else
	{
		// Generate New Data

		Block****& _blocks = _chunkData->blocks;
		_blocks = new Block ***[Chunk_Size];

		for (size_t x = 0; x < Chunk_Size; ++x)
		{
			Block***& _blocksX = _blocks[x];
			_blocksX = new Block **[Chunk_Size];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksXY = _blocksX[y];
				_blocksXY = new Block *[Chunk_Size];

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(x + _chunkWorldPosition.x, z + _chunkWorldPosition.z);
					if (_noiseHeight < _chunkWorldPosition.y + y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksXY[z] = new Block(_type);
				}
			}
		}
	}

	GenerateChunkSideData(_noiseCopy, _ownerChunk);
}

void Chunk_Data_Generator::GenerateChunkSideData(const Perlin_Noise& _noiseCopy, Chunk* _chunk)
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
	Chunk_SideData* _chunkSideData = _chunk->chunkSideData;
	const glm::vec3& _chunkWorldPosition = _chunk->worldPosition;
	ReleaseMutex(mutex_ChunkManager);

	const float& _ownerChunkHeight = _chunkWorldPosition.y;

	//Down Side
	if (_ownerChunkHeight - 1 > Chunk_Min_World_Height)
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _downWorldPosition = _chunkWorldPosition + glm::vec3(0.f, -1.f, 0.f);

			Block***& _blocks = _chunkSideData->downBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_downWorldPosition.x + x, _downWorldPosition.z + z);
					if (_noiseHeight < _downWorldPosition.y/* + Chunk_Max_Size*/)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[z] = new Block(_type);
				}
			}
		}
	}

	//Up Side
	if (_ownerChunkHeight + 1 < Chunk_Max_World_Height)
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _upWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 16.f, 0.f);

			Block***& _blocks = _chunkSideData->upBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_upWorldPosition.x + x, _upWorldPosition.z + z);
					if (_noiseHeight < _upWorldPosition.y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[z] = new Block(_type);
				}
			}
		}
	}

	//Left Side
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _leftWorldPosition = _chunkWorldPosition + glm::vec3(-1.f, 0.f, 0.f);

			Block***& _blocks = _chunkSideData->leftBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksX = _blocks[y];
				_blocksX = new Block *[Chunk_Size];

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_leftWorldPosition.x/* + Chunk_Max_Size*/, _leftWorldPosition.z + z);
					if (_noiseHeight < _leftWorldPosition.y + y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[z] = new Block(_type);
				}
			}
		}
	}

	//Right Side
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _rightWorldPosition = _chunkWorldPosition + glm::vec3(16.f, 0.f, 0.f);

			Block***& _blocks = _chunkSideData->rightBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksX = _blocks[y];
				_blocksX = new Block *[Chunk_Size];

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_rightWorldPosition.x, _rightWorldPosition.z + z);
					if (_noiseHeight < _rightWorldPosition.y + y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[z] = new Block(_type);
				}
			}
		}
	}

	//Back Side
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _backWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 0.f, -1.f);

			Block***& _blocks = _chunkSideData->backBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				for (size_t y = 0; y < Chunk_Size; ++y)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_backWorldPosition.x + x, _backWorldPosition.z/* + Chunk_Max_Size*/);
					if (_noiseHeight < _backWorldPosition.y + y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[y] = new Block(_type);
				}
			}
		}
	}

	//Front Side
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			const glm::vec3& _frontWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 0.f, 16.f);

			Block***& _blocks = _chunkSideData->frontBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				for (size_t y = 0; y < Chunk_Size; ++y)
				{
					EBlock_Type _type;

					int _noiseHeight = _noiseCopy.CalculateNoise(_frontWorldPosition.x + x, _frontWorldPosition.z);
					if (_noiseHeight < _frontWorldPosition.y + y)
					{
						_type = EBlock_Type::Air;
					}
					else
					{
						_type = (EBlock_Type)(rand() % 4 + 2); //Always full block  TODO generate with Noise
					}

					_blocksX[y] = new Block(_type);
				}
			}
		}
	}
}

Threaded void Chunk_Data_Generator::SetSideChunks(Chunk_Data*& _chunkData) const
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);

	Chunk*& _ownerChunk = _chunkData->ownerChunk;
	const glm::vec3& _ownerChunkPosition = _ownerChunk->GetChunkPosition();

	ReleaseMutex(mutex_ChunkManager);

	const float& _ownerChunkHeight =  _ownerChunkPosition.y;
	const glm::vec3& _downPosition =  _ownerChunkPosition + glm::vec3(0, -1, 0);
	const glm::vec3& _upPosition =    _ownerChunkPosition + glm::vec3(0, 1, 0);
	const glm::vec3& _leftPosition =  _ownerChunkPosition + glm::vec3(-1, 0, 0);
	const glm::vec3& _rightPosition = _ownerChunkPosition + glm::vec3(1, 0, 0);
	const glm::vec3& _backPosition =  _ownerChunkPosition + glm::vec3(0, 0, -1);
	const glm::vec3& _frontPosition = _ownerChunkPosition + glm::vec3(0, 0, 1);

	bool _needToWait = false;

	WaitForSingleObject(mutex_ChunkManager, INFINITE);

	if (_ownerChunkHeight > Chunk_Zero_World_Height)
	{
		if (Chunk* _downChunk = chunksManager->GetChunkAtPosition(_downPosition))
		{
			Chunk_Data* _downChunkData = _downChunk->chunkData;
			_downChunkData->upChunk = _ownerChunk;
			_chunkData->downChunk = _downChunk;
			_downChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (Chunk* _downChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_downPosition))
		{
			Chunk_Data* _downChunkData = _downChunk->chunkData;
			_downChunkData->upChunk = _ownerChunk;
			_chunkData->downChunk = _downChunk;
			_downChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_downPosition))
		{
			_chunkData->chunkPositionToWait.push_back(_downPosition);
			_needToWait = true;
		}
	}
	if (_ownerChunkHeight < Chunk_Max_World_Height)
	{
		if (Chunk* _upChunk = chunksManager->GetChunkAtPosition(_upPosition))
		{
			Chunk_Data* _upChunkData = _upChunk->chunkData;
			_upChunk->chunkData->downChunk = _ownerChunk;
			_chunkData->upChunk = _upChunk;
			_upChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (Chunk* _upChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_upPosition))
		{
			Chunk_Data* _upChunkData = _upChunk->chunkData;
			_upChunk->chunkData->downChunk = _ownerChunk;
			_chunkData->upChunk = _upChunk;
			_upChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_upPosition))
		{
			_chunkData->chunkPositionToWait.push_back(_upPosition);
			_needToWait = true;
		}
	}

	if (Chunk* _leftChunk = chunksManager->GetChunkAtPosition(_leftPosition))
	{
		Chunk_Data* _leftChunkData = _leftChunk->chunkData;
		_leftChunk->chunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
		_leftChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _leftChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_leftPosition))
	{
		Chunk_Data* _leftChunkData = _leftChunk->chunkData;
		_leftChunk->chunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
		_leftChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_leftPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_leftPosition);
		_needToWait = true;
	}


	if (Chunk* _rightChunk = chunksManager->GetChunkAtPosition(_rightPosition))
	{
		Chunk_Data* _rightChunkData = _rightChunk->chunkData;
		_rightChunk->chunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
		_rightChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _rightChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_rightPosition))
	{
		Chunk_Data* _rightChunkData = _rightChunk->chunkData;
		_rightChunk->chunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
		_rightChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_rightPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_rightPosition);
		_needToWait = true;
	}


	if (Chunk* _backChunk = chunksManager->GetChunkAtPosition(_backPosition))
	{
		Chunk_Data* _backChunkData = _backChunk->chunkData;
		_backChunk->chunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
		_backChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _backChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_backPosition))
	{
		Chunk_Data* _backChunkData = _backChunk->chunkData;
		_backChunk->chunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
		_backChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_backPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_backPosition);
		_needToWait = true;
	}


	if (Chunk* _frontChunk = chunksManager->GetChunkAtPosition(_frontPosition))
	{
		Chunk_Data* _frontChunkData = _frontChunk->chunkData;
		_frontChunk->chunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
		_frontChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _frontChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_frontPosition))
	{
		Chunk_Data* _frontChunkData = _frontChunk->chunkData;
		_frontChunk->chunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
		_frontChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_frontPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_frontPosition);
		_needToWait = true;
	}


	if (_needToWait)
	{
		chunksManager->onChunkInitialized.AddDynamic(_chunkData, &Chunk_Data::AddSideChunk);
	}
	else
	{
		_chunkData->bHasFinishWaitSideChunk = true;
	}

	ReleaseMutex(mutex_ChunkManager);
}