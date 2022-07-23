#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "Block.h"

Chunk_Data::Chunk_Data()
{
	
}

Chunk_Data::~Chunk_Data()
{
	for (size_t x = 0; x < Chunk_Size; x++)
	{
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				delete blocks[x][y][z];
			}
		}
	}
}