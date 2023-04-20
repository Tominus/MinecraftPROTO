#include "Chunk_Data_Generator.h"

#include <vector>
#include <glm/detail/type_vec.hpp>

#include "Chunks_Manager.h"
#include "Chunk_SideData.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Chunk.h"
#include "Action.h"
#include "PerlinNoise.h"

Chunk_Data_Generator::Chunk_Data_Generator(Chunks_Manager* _chunksManager)
{
	perlinNoise = &Perlin_Noise::Instance();
	chunksManager = _chunksManager;
	mutex_ChunkManager = _chunksManager->mutex;
	mutex_ChunkDataGenerator = CreateMutex(0, false, 0);
	randMax = (unsigned)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;
	fMinChunkHeight = Chunk_Min_World_Height + 1;
	fMaxChunkHeight = Chunk_Max_World_Height - 1;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{
	ReleaseMutex(mutex_ChunkDataGenerator);
}

void Chunk_Data_Generator::GenerateNewChunkData(Chunk_Data*& _chunkData)
{
	WaitForSingleObject(mutex_ChunkDataGenerator, INFINITE);
	Chunk* _ownerChunk = _chunkData->ownerChunk;
	Chunk_Render* _ownerChunkRender = _ownerChunk->chunkRender;
	glm::vec3 _chunkWorldPosition(_ownerChunk->worldPosition);
	ReleaseMutex(mutex_ChunkDataGenerator);

	Block****& _blocks = _chunkData->blocks;


	if (/*Exist in file*/false)
	{
		// Load Data
	}
	else
	{
		// Generate New Data

		_blocks = new Block ***[Chunk_Size];

		for (size_t x = 0; x < Chunk_Size; ++x)
		{
			Block***& _blocksX = _blocks[x];
			_blocksX = new Block **[Chunk_Size];

			_chunkWorldPosition.x += x;

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksXY = _blocksX[y];
				_blocksXY = new Block *[Chunk_Size];

				_chunkWorldPosition.y += y;

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					_chunkWorldPosition.z += z;

					_blocksXY[z] = GenerateBlock(_chunkWorldPosition);

					_chunkWorldPosition.z -= z;
				}

				_chunkWorldPosition.y -= y;
			}

			_chunkWorldPosition.x -= x;
		}
	}

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block***& _blocksX = _blocks[x];
		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Block**& _blocksXY = _blocksX[y];
			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				if (_blocksXY[z]->blockType != EBlock_Type::Air)
				{
					_ownerChunkRender->bHasRender = true; 
					// It is used in Render_Generator to check if it has more than 0 render shape

					GenerateChunkSideData(_ownerChunk);
					return;
				}
			}
		}
	}
}

void Chunk_Data_Generator::GenerateChunkSideData(Chunk* _chunk)
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
	Chunk_SideData* _chunkSideData = _chunk->chunkSideData;
	glm::vec3 _chunkWorldPosition(_chunk->worldPosition);
	float _chunkHeightPosition = _chunk->chunkPosition.y;
	float _ownerChunkHeight = _chunkWorldPosition.y;
	ReleaseMutex(mutex_ChunkManager);

	//Down Side
	if (_chunkHeightPosition > fMinChunkHeight)
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			glm::vec3& _downWorldPosition = _chunkWorldPosition + glm::vec3(0.f, -1.f, 0.f);

			Block***& _blocks = _chunkSideData->downBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				_downWorldPosition.x += x;

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					_downWorldPosition.z += z;

					_blocksX[z] = GenerateBlock(_downWorldPosition);

					_downWorldPosition.z -= z;
				}

				_downWorldPosition.x -= x;
			}
		}
	}

	//Up Side
	if (_chunkHeightPosition < fMaxChunkHeight)
	{
		if (/*Exist in file*/false)
		{

		}
		else
		{
			glm::vec3& _upWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 16.f, 0.f);

			Block***& _blocks = _chunkSideData->upBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				_upWorldPosition.x += x;

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					_upWorldPosition.z += z;

					_blocksX[z] = GenerateBlock(_upWorldPosition);

					_upWorldPosition.z -= z;
				}

				_upWorldPosition.x -= x;
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
			glm::vec3& _leftWorldPosition = _chunkWorldPosition + glm::vec3(-1.f, 0.f, 0.f);

			Block***& _blocks = _chunkSideData->leftBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksX = _blocks[y];
				_blocksX = new Block *[Chunk_Size];

				_leftWorldPosition.y += y;

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					_leftWorldPosition.z += z;

					_blocksX[z] = GenerateBlock(_leftWorldPosition);

					_leftWorldPosition.z -= z;
				}

				_leftWorldPosition.y -= y;
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
			glm::vec3& _rightWorldPosition = _chunkWorldPosition + glm::vec3(16.f, 0.f, 0.f);

			Block***& _blocks = _chunkSideData->rightBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Block**& _blocksX = _blocks[y];
				_blocksX = new Block *[Chunk_Size];

				_rightWorldPosition.y += y;

				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					_rightWorldPosition.z += z;

					_blocksX[z] = GenerateBlock(_rightWorldPosition);

					_rightWorldPosition.z -= z;
				}

				_rightWorldPosition.y -= y;
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
			glm::vec3& _backWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 0.f, -1.f);

			Block***& _blocks = _chunkSideData->backBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				_backWorldPosition.x += x;

				for (size_t y = 0; y < Chunk_Size; ++y)
				{
					_backWorldPosition.y += y;

					_blocksX[y] = GenerateBlock(_backWorldPosition);

					_backWorldPosition.y -= y;
				}

				_backWorldPosition.x -= x;
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
			glm::vec3& _frontWorldPosition = _chunkWorldPosition + glm::vec3(0.f, 0.f, 16.f);

			Block***& _blocks = _chunkSideData->frontBlocks;
			_blocks = new Block **[Chunk_Size];

			for (size_t x = 0; x < Chunk_Size; ++x)
			{
				Block**& _blocksX = _blocks[x];
				_blocksX = new Block *[Chunk_Size];

				_frontWorldPosition.x += x;

				for (size_t y = 0; y < Chunk_Size; ++y)
				{
					_frontWorldPosition.y += y;

					_blocksX[y] = GenerateBlock(_frontWorldPosition);

					_frontWorldPosition.y -= y;
				}

				_frontWorldPosition.x -= x;
			}
		}
	}
}

Threaded void Chunk_Data_Generator::SetSideChunks(Chunk_Data*& _chunkData) const
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
	Chunk*& _ownerChunk = _chunkData->ownerChunk;
	glm::vec3 _ownerChunkPosition(_ownerChunk->chunkPosition);
	float _ownerChunkHeight =  _ownerChunkPosition.y;
	ReleaseMutex(mutex_ChunkManager);

	const glm::vec3& _downPosition =  _ownerChunkPosition + glm::vec3(0, -1, 0);
	const glm::vec3& _upPosition =    _ownerChunkPosition + glm::vec3(0, 1, 0);
	const glm::vec3& _leftPosition =  _ownerChunkPosition + glm::vec3(-1, 0, 0);
	const glm::vec3& _rightPosition = _ownerChunkPosition + glm::vec3(1, 0, 0);
	const glm::vec3& _backPosition =  _ownerChunkPosition + glm::vec3(0, 0, -1);
	const glm::vec3& _frontPosition = _ownerChunkPosition + glm::vec3(0, 0, 1);

	bool _needToWait = false;

	WaitForSingleObject(mutex_ChunkManager, INFINITE);

	if (_ownerChunkHeight > Chunk_Min_Limit_World_Height)
	{
		if (Chunk* _downChunk = chunksManager->Opti_GetChunk(_downPosition))
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
	if (_ownerChunkHeight < Chunk_Max_Limit_World_Height)
	{
		if (Chunk* _upChunk = chunksManager->Opti_GetChunk(_upPosition))
		{
			Chunk_Data* _upChunkData = _upChunk->chunkData;
			_upChunkData->downChunk = _ownerChunk;
			_chunkData->upChunk = _upChunk;
			_upChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (Chunk* _upChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_upPosition))
		{
			Chunk_Data* _upChunkData = _upChunk->chunkData;
			_upChunkData->downChunk = _ownerChunk;
			_chunkData->upChunk = _upChunk;
			_upChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
		}
		else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_upPosition))
		{
			_chunkData->chunkPositionToWait.push_back(_upPosition);
			_needToWait = true;
		}
	}

	if (Chunk* _leftChunk = chunksManager->Opti_GetChunk(_leftPosition))
	{
		Chunk_Data* _leftChunkData = _leftChunk->chunkData;
		_leftChunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
		_leftChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _leftChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_leftPosition))
	{
		Chunk_Data* _leftChunkData = _leftChunk->chunkData;
		_leftChunkData->rightChunk = _ownerChunk;
		_chunkData->leftChunk = _leftChunk;
		_leftChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_leftPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_leftPosition);
		_needToWait = true;
	}


	if (Chunk* _rightChunk = chunksManager->Opti_GetChunk(_rightPosition))
	{
		Chunk_Data* _rightChunkData = _rightChunk->chunkData;
		_rightChunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
		_rightChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _rightChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_rightPosition))
	{
		Chunk_Data* _rightChunkData = _rightChunk->chunkData;
		_rightChunkData->leftChunk = _ownerChunk;
		_chunkData->rightChunk = _rightChunk;
		_rightChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_rightPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_rightPosition);
		_needToWait = true;
	}


	if (Chunk* _backChunk = chunksManager->Opti_GetChunk(_backPosition))
	{
		Chunk_Data* _backChunkData = _backChunk->chunkData;
		_backChunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
		_backChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _backChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_backPosition))
	{
		Chunk_Data* _backChunkData = _backChunk->chunkData;
		_backChunkData->frontChunk = _ownerChunk;
		_chunkData->backChunk = _backChunk;
		_backChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (chunksManager->GetIsChunkAtPositionBeingGenerated(_backPosition))
	{
		_chunkData->chunkPositionToWait.push_back(_backPosition);
		_needToWait = true;
	}


	if (Chunk* _frontChunk = chunksManager->Opti_GetChunk(_frontPosition))
	{
		Chunk_Data* _frontChunkData = _frontChunk->chunkData;
		_frontChunkData->backChunk = _ownerChunk;
		_chunkData->frontChunk = _frontChunk;
		_frontChunkData->AddGeneratedSideChunk(_ownerChunkPosition);
	}
	else if (Chunk* _frontChunk = chunksManager->GetChunkBeingGeneratedAtPosition(_frontPosition))
	{
		Chunk_Data* _frontChunkData = _frontChunk->chunkData;
		_frontChunkData->backChunk = _ownerChunk;
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