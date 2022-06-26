#include "Chunk.h"

#include "GlobalDefine.h"
#include "World.h"
#include "TextureLoader.h"
#include "Block_Render.h"

Chunk::Chunk(const GLuint& _matrixID, const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * 16.0f;
	const World* _world = &World::Instance();
	const TextureLoader* textureLoader = _world->GetTextureLoader();
	const uint& _randMax = (uint)Block_Type::Red_Stained_Glass + 1;

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		std::vector<std::vector<Block_Render*>> _blocksX;
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			std::vector<Block_Render*> _blocksXY;
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				const Block_Type& _type = (Block_Type)(rand() % _randMax);
				Block_Render* _block = new Block_Render(textureLoader->GetBlockTextureID(_type), _matrixID);
				_blocksXY.push_back(_block);

				//--- DELETE
				_block->position = worldPosition + glm::vec3(x, y, z);

				if (_type == Block_Type::Glass || _type == Block_Type::Red_Stained_Glass)
				{
					alphaBlocks.push_back(_block);
				}
				else
				{
					normalBlocks.push_back(_block);
				}
			}

			_blocksX.push_back(_blocksXY);
		}

		blocks.push_back(_blocksX);
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
	glm::vec3 _worldPosition(worldPosition);

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				Block_Render* _block = blocks[x][y][z];
				if (_block->textureID != 5)
					_block->Render(_worldPosition);
				_worldPosition.z++;
			}
			_worldPosition.z = worldPosition.z;
			_worldPosition.y++;
		}
		_worldPosition.y = worldPosition.y;
		_worldPosition.x++;
	}
}

void Chunk::Render_Alpha()
{
	glm::vec3 _worldPosition(worldPosition);

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				Block_Render* _block = blocks[x][y][z];
				if (_block->textureID == 5)
					_block->Render_Alpha(_worldPosition);
				_worldPosition.z++;
			}
			_worldPosition.z = worldPosition.z;
			_worldPosition.y++;
		}
		_worldPosition.y = worldPosition.y;
		_worldPosition.x++;
	}
}

void Chunk::Render_NormalBlock()
{
	const size_t& _max = normalBlocks.size();
	for (size_t i = 0; i < _max; i++)
	{
		normalBlocks[i]->Render();
	}
}

void Chunk::Render_AlphaBlock()
{
	const size_t& _max = alphaBlocks.size();
	for (size_t i = 0; i < _max; i++)
	{
		alphaBlocks[i]->Render_Alpha();
	}
}