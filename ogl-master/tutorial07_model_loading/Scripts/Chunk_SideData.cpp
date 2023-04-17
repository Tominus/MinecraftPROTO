#include "Chunk_SideData.h"
#include "Block.h"
#include "GlobalDefine.h"

Chunk_SideData::Chunk_SideData()
{
	downBlocks = nullptr;
	upBlocks = nullptr;
	leftBlocks = nullptr;
	rightBlocks = nullptr;
	backBlocks = nullptr;
	frontBlocks = nullptr;
}

Chunk_SideData::~Chunk_SideData()
{
	if (downBlocks)
	{
		for (size_t x = 0; x < Chunk_Size; ++x)
		{
			Block** _d = downBlocks[x];
			for (size_t y = 0; y < Chunk_Size; ++y)
				delete _d[y];
			delete[] _d;
		}
		delete[] downBlocks;
	}
	if (upBlocks)
	{
		for (size_t x = 0; x < Chunk_Size; ++x)
		{
			Block** _u = upBlocks[x];
			for (size_t y = 0; y < Chunk_Size; ++y)
				delete _u[y];
			delete[] _u;
		}
		delete[] upBlocks;
	}

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block** _l = leftBlocks[x];
		Block** _r = rightBlocks[x];
		Block** _b = backBlocks[x];
		Block** _f = frontBlocks[x];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			delete _l[y];
			delete _r[y];
			delete _b[y];
			delete _f[y];
		}

		delete[] _l;
		delete[] _r;
		delete[] _b;
		delete[] _f;
	}

	delete[] leftBlocks;
	delete[] rightBlocks;
	delete[] backBlocks;
	delete[] frontBlocks;
}