#include "Chunk_Data_Generator.h"

#include <vector>

#include "GlobalDefine.h"
#include "Chunk_Data.h"
#include "Block.h"
#include "Block_Type.h"


Chunk_Data_Generator::Chunk_Data_Generator()
{
	randMax = (unsigned)Block_Type::BLOCK_TYPE_MAX_NUMBER;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{

}

void Chunk_Data_Generator::GenerateNewChunkData(Chunk_Data* _chunkData) const
{
	std::vector<std::vector<std::vector<Block*>>>& _blocks = _chunkData->blocks;

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		std::vector<std::vector<Block*>> _blocksX;
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			std::vector<Block*> _blocksXY;
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				const Block_Type& _type = (Block_Type)(rand() % randMax);
				_blocksXY.push_back(new Block(_type));
			}

			_blocksX.push_back(_blocksXY);
		}

		_blocks.push_back(_blocksX);
	}
}