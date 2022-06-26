#include "Chunk_Data.h"

#include "GlobalDefine.h"
#include "World.h"
#include "Block.h"
#include "Block_Type.h"


Chunk_Data::Chunk_Data()
{
	const World* _world = &World::Instance();
	const TextureLoader* textureLoader = _world->GetTextureLoader();
	const unsigned& _randMax = (unsigned)Block_Type::BLOCK_TYPE_MAX_NUMBER;

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		std::vector<std::vector<Block*>> _blocksX;
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			std::vector<Block*> _blocksXY;
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				const Block_Type& _type = (Block_Type)(rand() % _randMax);
				_blocksXY.push_back(new Block(_type));
			}

			_blocksX.push_back(_blocksXY);
		}

		blocks.push_back(_blocksX);
	}
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