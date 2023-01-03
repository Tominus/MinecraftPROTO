#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"
#include "Chunk.h"

Chunk_Data::Chunk_Data(Chunk* _ownerChunk)
{
	ownerChunk = _ownerChunk;
	blocks = nullptr;
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