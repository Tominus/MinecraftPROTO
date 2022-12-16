#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"

Chunk_Data::Chunk_Data()
{
	blocks = nullptr;
}

Chunk_Data::~Chunk_Data()
{
	/*for (size_t x = 0; x < Chunk_Size; ++x)
	{
		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				delete blocks[x][y][z];
			}
		}
	}*/

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