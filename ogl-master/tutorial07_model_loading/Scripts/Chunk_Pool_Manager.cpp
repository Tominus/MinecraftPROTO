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

	mutex_ChunkPool = CreateMutex(0, false, 0);
}

Chunk_Pool_Manager::~Chunk_Pool_Manager()
{
	CloseHandle(mutex_ChunkPool);
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
}

void Chunk_Pool_Manager::DestroyAllChunkData()
{
	const size_t& _size = chunkPool.size();
	for (size_t i = 0; i < _size; ++i)
	{
		delete chunkPool[i];
	}
}

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

		for (size_t z = 0; z < Chunk_Size; ++z)
		{
			_blocksDX[z]->blockType = EBlock_Type::Air;
			_blocksUX[z]->blockType = EBlock_Type::Air;
			_blocksLX[z]->blockType = EBlock_Type::Air;
			_blocksRX[z]->blockType = EBlock_Type::Air;
			_blocksBX[z]->blockType = EBlock_Type::Air;
			_blocksFX[z]->blockType = EBlock_Type::Air;
		}
	}
}