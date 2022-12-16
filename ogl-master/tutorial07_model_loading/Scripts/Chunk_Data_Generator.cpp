#include "Chunk_Data_Generator.h"

#include <vector>
#include <random>

#include "GlobalDefine.h"
#include "Chunk_Data.h"
#include "Block.h"
#include "Block_Type.h"

Chunk_Data_Generator::Chunk_Data_Generator()
{
	randMax = (unsigned)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;
}

Chunk_Data_Generator::~Chunk_Data_Generator()
{

}

void Chunk_Data_Generator::GenerateNewChunkData(Chunk_Data* _chunkData) const
{
	std::random_device _rd;
	std::mt19937 _gen(_rd());
	std::uniform_int_distribution<> _dist(0, randMax - 1);

	Block****& _blocks = _chunkData->blocks;
	_blocks = new Block***[Chunk_Size];

	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		Block***& _blocksX = _blocks[x];
		_blocksX = new Block**[Chunk_Size];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Block**& _blocksXY = _blocksX[y];
			_blocksXY = new Block*[Chunk_Size];

			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				const EBlock_Type& _type = (EBlock_Type)(_dist(_gen));
				//const EBlock_Type& _type = (EBlock_Type)(rand() % randMax);
				_blocksXY[z] = new Block(_type);
			}
		}
	}
}