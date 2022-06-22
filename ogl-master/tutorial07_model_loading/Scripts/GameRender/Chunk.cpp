#include "Chunk.h"
#include "tutorial07_model_loading/Scripts/GlobalDefine.h"

Chunk::Chunk(GLuint _textureID, GLuint _matrixID)
{
	for (size_t x = 0; x < Chunk_Size; x++)
	{
		std::vector<std::vector<Block_Render*>> blocksX;

		for (size_t y = 0; y < Chunk_Size; y++)
		{
			std::vector<Block_Render*> blocksXY;

			for (size_t z = 0; z < Chunk_Size; z++)
			{
				blocksXY.push_back(new Block_Render(_textureID, _matrixID));
			}

			blocksX.push_back(blocksXY);
		}

		blocks.push_back(blocksX);
	}
}

Chunk::~Chunk()
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

void Chunk::Render()
{
	for (size_t x = 0; x < Chunk_Size; x++)
	{		
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				blocks[x][y][z]->Render();
			}
		}
	}
}