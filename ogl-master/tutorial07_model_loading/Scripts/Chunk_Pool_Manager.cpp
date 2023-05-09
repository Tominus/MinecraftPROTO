#include "Chunk_Pool_Manager.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glfw3.h>

#include "GlobalDefine.h"
#include "Block.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Chunk_SideData.h"
#include "Thread_Structs.h"


Chunk_Pool_Manager::Chunk_Pool_Manager(Chunks_Manager* _chunkManager, Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator)
{
	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;
	chunkManager = _chunkManager;

	bHasToGenerateNewChunkRenderBuffer = false;
	bHasToGenerateNewChunkRenderData = false;
	bHasToGenerateNewChunkRenderShapes = false;

	mutex_ChunkPool = CreateMutex(0, false, 0);
	mutex_ChunkRenderBufferPool = CreateMutex(0, false, 0);
	mutex_ChunkRenderDataPool = CreateMutex(0, false, 0);
	mutex_ChunkRenderShapesPool = CreateMutex(0, false, 0);
	mutex_HasToGenerateData = CreateMutex(0, false, 0);
}

Chunk_Pool_Manager::~Chunk_Pool_Manager()
{
	CloseHandle(mutex_ChunkPool);
	CloseHandle(mutex_ChunkRenderBufferPool);
	CloseHandle(mutex_ChunkRenderDataPool);
	CloseHandle(mutex_ChunkRenderShapesPool);
	CloseHandle(mutex_HasToGenerateData);
}

void Chunk_Pool_Manager::InitializeAllChunkData()
{
	const size_t& _size = Render_Distance_Total * Render_Distance_Total * Chunk_Max_World_Height;
	for (size_t i = 0; i < _size; ++i)
	{
		Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator);

		Chunk_Data* _chunkData = new Chunk_Data(_chunk);
		Chunk_Render* _chunkRender = new Chunk_Render(_chunk);
		Chunk_SideData* _chunkSideData = new Chunk_SideData();

		GenerateChunkHandle(_chunk);
		GenerateChunkData(_chunkData);
		GenerateChunkRender(_chunkRender);
		GenerateChunkSideData(_chunkSideData);

		_chunk->chunkData = _chunkData;
		_chunk->chunkRender = _chunkRender;
		_chunk->chunkSideData = _chunkSideData;

		chunkPool.push_back(_chunk);
	}

	GenerateChunkRenderBuffer();
	GenerateChunkRenderData();
	GenerateChunkRenderShapes();
}

void Chunk_Pool_Manager::DestroyAllChunkData()
{
	const size_t& _chunkPoolSize = chunkPool.size();
	for (size_t i = 0; i < _chunkPoolSize; ++i)
	{
		delete chunkPool[i];
	}

	const size_t& _chunkRenderBufferPoolSize = chunkRenderBufferPool.size();
	for (size_t i = 0; i < _chunkRenderBufferPoolSize; ++i)
	{
		delete chunkRenderBufferPool[i];
	}

	const size_t& _chunkRenderDataPoolSize = chunkRenderDataPool.size();
	for (size_t i = 0; i < _chunkRenderDataPoolSize; ++i)
	{
		delete chunkRenderDataPool[i];
	}

	const size_t& _chunkRenderShapesPoolSize = chunkRenderShapesPool.size();
	for (size_t i = 0; i < _chunkRenderShapesPoolSize; ++i)
	{
		delete chunkRenderShapesPool[i];
	}
}

#pragma region Generate
void Chunk_Pool_Manager::GenerateChunkHandle(Chunk* _chunk)
{
	_chunk->handle_AddChunk = new SThread_AddChunk(nullptr, chunkManager, glm::vec3());
}

void Chunk_Pool_Manager::GenerateChunkData(Chunk_Data* _chunkData)
{
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
				_blocksXY[z] = new Block(EBlock_Type::Air);
			}
		}
	}
}

void Chunk_Pool_Manager::GenerateChunkRender(Chunk_Render* _chunkRender)
{
	_chunkRender->bHasFinishGeneration = false;
	_chunkRender->bHasRender = false;

	SChunk_Render_Shapes****& _allBlockShapes = _chunkRender->allBlockShapes;
	_allBlockShapes = new SChunk_Render_Shapes ***[Chunk_Size];

	for (int x = 0; x < Chunk_Size; ++x)
	{
		SChunk_Render_Shapes***& _currentRenderShapesX = _allBlockShapes[x];
		_currentRenderShapesX = new SChunk_Render_Shapes **[Chunk_Size];

		for (int y = 0; y < Chunk_Size; ++y)
		{
			SChunk_Render_Shapes**& _currentRenderShapesY = _currentRenderShapesX[y];
			_currentRenderShapesY = new SChunk_Render_Shapes *[Chunk_Size];

			for (int z = 0; z < Chunk_Size; ++z)
			{
				_currentRenderShapesY[z] = nullptr;
			}
		}
	}
}

void Chunk_Pool_Manager::GenerateChunkSideData(Chunk_SideData* _chunkSideData)
{
	Block***& _blocksD = _chunkSideData->downBlocks;
	Block***& _blocksU = _chunkSideData->upBlocks;
	Block***& _blocksL = _chunkSideData->leftBlocks;
	Block***& _blocksR = _chunkSideData->rightBlocks;
	Block***& _blocksB = _chunkSideData->backBlocks;
	Block***& _blocksF = _chunkSideData->frontBlocks;

	_blocksD = new Block **[Chunk_Size];
	_blocksU = new Block **[Chunk_Size];
	_blocksL = new Block **[Chunk_Size];
	_blocksR = new Block **[Chunk_Size];
	_blocksB = new Block **[Chunk_Size];
	_blocksF = new Block **[Chunk_Size];

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block**& _blocksDX = _blocksD[x];
		Block**& _blocksUX = _blocksU[x];
		Block**& _blocksLX = _blocksL[x];
		Block**& _blocksRX = _blocksR[x];
		Block**& _blocksBX = _blocksB[x];
		Block**& _blocksFX = _blocksF[x];

		_blocksDX = new Block *[Chunk_Size];
		_blocksUX = new Block *[Chunk_Size];
		_blocksLX = new Block *[Chunk_Size];
		_blocksRX = new Block *[Chunk_Size];
		_blocksBX = new Block *[Chunk_Size];
		_blocksFX = new Block *[Chunk_Size];

		for (size_t z = 0; z < Chunk_Size; ++z)
		{
			_blocksDX[z] = new Block(EBlock_Type::Air);
			_blocksUX[z] = new Block(EBlock_Type::Air);
			_blocksLX[z] = new Block(EBlock_Type::Air);
			_blocksRX[z] = new Block(EBlock_Type::Air);
			_blocksBX[z] = new Block(EBlock_Type::Air);
			_blocksFX[z] = new Block(EBlock_Type::Air);
		}
	}
}

void Chunk_Pool_Manager::GenerateChunkRenderBuffer()
{
	//TODO calculate how many i need with different Render distance
}

void Chunk_Pool_Manager::GenerateChunkRenderData()
{
	//TODO calculate how many i need with different Render distance
}

void Chunk_Pool_Manager::GenerateChunkRenderShapes()
{
	//TODO calculate how many i need with different Render distance
}
#pragma endregion

#pragma region Clear
void Chunk_Pool_Manager::ClearChunkHandle(Chunk* _chunk)
{
	SThread_AddChunk_Ptr _addChunk = _chunk->handle_AddChunk;
	_addChunk->thisThread = nullptr;
}

void Chunk_Pool_Manager::ClearChunkData(Chunk_Data* _chunkData)
{
	Block****& _blocks = _chunkData->blocks;

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block***& _blocksX = _blocks[x];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Block**& _blocksXY = _blocksX[y];

			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				_blocksXY[z]->blockType = EBlock_Type::Air;
			}
		}
	}

	_chunkData->bHasFinishWaitSideChunk = false;
	_chunkData->chunkPositionToWait.clear();
}

void Chunk_Pool_Manager::ClearChunkRender(Chunk_Render* _chunkRender)
{

}

void Chunk_Pool_Manager::ClearChunkSideData(Chunk_SideData* _chunkSideData)
{
	_chunkSideData->bUseChunkSideData = false;

	Block***& _blocksD = _chunkSideData->downBlocks;
	Block***& _blocksU = _chunkSideData->upBlocks;
	Block***& _blocksL = _chunkSideData->leftBlocks;
	Block***& _blocksR = _chunkSideData->rightBlocks;
	Block***& _blocksB = _chunkSideData->backBlocks;
	Block***& _blocksF = _chunkSideData->frontBlocks;

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block**& _blocksDX = _blocksD[x];
		Block**& _blocksUX = _blocksU[x];
		Block**& _blocksLX = _blocksL[x];
		Block**& _blocksRX = _blocksR[x];
		Block**& _blocksBX = _blocksB[x];
		Block**& _blocksFX = _blocksF[x];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			_blocksDX[y]->blockType = EBlock_Type::Air;
			_blocksUX[y]->blockType = EBlock_Type::Air;
			_blocksLX[y]->blockType = EBlock_Type::Air;
			_blocksRX[y]->blockType = EBlock_Type::Air;
			_blocksBX[y]->blockType = EBlock_Type::Air;
			_blocksFX[y]->blockType = EBlock_Type::Air;
		}
	}
}
#pragma endregion

#pragma region Update
void Chunk_Pool_Manager::Update()
{
	WaitForSingleObject(mutex_HasToGenerateData, INFINITE);

	CheckHasToGenerateNewChunkRenderBuffer();
	CheckHasToGenerateNewChunkRenderData();
	CheckHasToGenerateNewChunkRenderShapes();

	ReleaseMutex(mutex_HasToGenerateData);
}

void Chunk_Pool_Manager::CheckHasToGenerateNewChunkRenderBuffer()
{
	if (bHasToGenerateNewChunkRenderBuffer)
	{
		WaitForSingleObject(mutex_ChunkRenderBufferPool, INFINITE);
		for (size_t i = 0; i < 64; ++i)
		{
			chunkRenderBufferPool.push_back(new SChunk_Render_Buffer(nullptr, glm::vec3()));
		}
		ReleaseMutex(mutex_ChunkRenderBufferPool);

		bHasToGenerateNewChunkRenderBuffer = false;
	}
}

void Chunk_Pool_Manager::CheckHasToGenerateNewChunkRenderData()
{
	if (bHasToGenerateNewChunkRenderData)
	{
		WaitForSingleObject(mutex_ChunkRenderDataPool, INFINITE);
		for (size_t i = 0; i < 4; ++i)
		{
			chunkRenderDataPool.push_back(new SChunk_Render_Data());
		}
		ReleaseMutex(mutex_ChunkRenderDataPool);

		bHasToGenerateNewChunkRenderData = false;
	}
}

void Chunk_Pool_Manager::CheckHasToGenerateNewChunkRenderShapes()
{
	if (bHasToGenerateNewChunkRenderShapes)
	{
		WaitForSingleObject(mutex_ChunkRenderShapesPool, INFINITE);
		for (size_t i = 0; i < 64; ++i)
		{
			chunkRenderShapesPool.push_back(new SChunk_Render_Shapes(nullptr, nullptr, nullptr));
		}
		ReleaseMutex(mutex_ChunkRenderShapesPool);

		bHasToGenerateNewChunkRenderShapes = false;
	}
}

void Chunk_Pool_Manager::SetHasToGenerateNewChunkRenderBuffer()
{
	WaitForSingleObject(mutex_HasToGenerateData, INFINITE);
	bHasToGenerateNewChunkRenderBuffer = true;
	ReleaseMutex(mutex_HasToGenerateData);
}

void Chunk_Pool_Manager::SetHasToGenerateNewChunkRenderData()
{
	WaitForSingleObject(mutex_HasToGenerateData, INFINITE);
	bHasToGenerateNewChunkRenderData = true;
	ReleaseMutex(mutex_HasToGenerateData);
}

void Chunk_Pool_Manager::SetHasToGenerateNewChunkRenderShapes()
{
	WaitForSingleObject(mutex_HasToGenerateData, INFINITE);
	bHasToGenerateNewChunkRenderShapes = true;
	ReleaseMutex(mutex_HasToGenerateData);
}
#pragma endregion

#pragma region Get && Retreive
Chunk* Chunk_Pool_Manager::GetChunk()
{
	Chunk* _chunk = nullptr;

	WaitForSingleObject(mutex_ChunkPool, INFINITE);
	if (chunkPool.size() > 0)
	{
		_chunk = chunkPool.back();
		chunkPool.pop_back();
	}
	ReleaseMutex(mutex_ChunkPool);

	return _chunk;
}

void Chunk_Pool_Manager::RetreiveChunk(Chunk* _chunk)
{
	//TODO data clear is maybe useless

	//ClearChunkHandle(_chunk);
	//ClearChunkData(_chunk->chunkData);
	//ClearChunkRender(_chunk->chunkRender);
	//ClearChunkSideData(_chunk->chunkSideData);

	WaitForSingleObject(mutex_ChunkPool, INFINITE);
	chunkPool.push_back(_chunk);
	ReleaseMutex(mutex_ChunkPool);
}

SChunk_Render_Buffer* Chunk_Pool_Manager::GetChunkRenderBuffer()
{
	SChunk_Render_Buffer* _buffer = nullptr;

	WaitForSingleObject(mutex_ChunkRenderBufferPool, INFINITE);
	if (chunkRenderBufferPool.size() > 0)
	{
		_buffer = chunkRenderBufferPool.back();
		chunkRenderBufferPool.pop_back();
	}
	ReleaseMutex(mutex_ChunkRenderBufferPool);

	return _buffer;
}

void Chunk_Pool_Manager::RetreiveChunkRenderBuffer(SChunk_Render_Buffer* _chunkRenderBuffer)
{
	WaitForSingleObject(mutex_ChunkRenderBufferPool, INFINITE);
	chunkRenderBufferPool.push_back(_chunkRenderBuffer);
	ReleaseMutex(mutex_ChunkRenderBufferPool);
}

SChunk_Render_Data* Chunk_Pool_Manager::GetChunkRenderData()
{
	SChunk_Render_Data* _data = nullptr;
	WaitForSingleObject(mutex_ChunkRenderDataPool, INFINITE);
	if (chunkRenderDataPool.size() > 0)
	{
		_data = chunkRenderDataPool.back();
		chunkRenderDataPool.pop_back();
	}
	ReleaseMutex(mutex_ChunkRenderDataPool);
	return _data;
}

void Chunk_Pool_Manager::RetreiveChunkRenderData(SChunk_Render_Data* _chunkRenderData)
{
	WaitForSingleObject(mutex_ChunkRenderDataPool, INFINITE);
	chunkRenderDataPool.push_back(_chunkRenderData);
	ReleaseMutex(mutex_ChunkRenderDataPool);
}

SChunk_Render_Shapes* Chunk_Pool_Manager::GetChunkRenderShapes()
{
	SChunk_Render_Shapes* _shapes = nullptr;
	WaitForSingleObject(mutex_ChunkRenderShapesPool, INFINITE);
	if (chunkRenderShapesPool.size() > 0)
	{
		_shapes = chunkRenderShapesPool.back();
		chunkRenderShapesPool.pop_back();
	}
	ReleaseMutex(mutex_ChunkRenderShapesPool);
	return _shapes;
}

void Chunk_Pool_Manager::RetreiveChunkRenderShapes(SChunk_Render_Shapes* _chunkRenderShapes)
{
	WaitForSingleObject(mutex_ChunkRenderShapesPool, INFINITE);
	chunkRenderShapesPool.push_back(_chunkRenderShapes);
	ReleaseMutex(mutex_ChunkRenderShapesPool);
}
#pragma endregion