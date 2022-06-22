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
	const int& _randMax = (int)Block_Type::Glass + 1;

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		std::vector<std::vector<Block_Render*>> blocksX;
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			std::vector<Block_Render*> blocksXY;
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				blocksXY.push_back(new Block_Render(textureLoader->GetBlockTextureID((Block_Type)(rand() % _randMax)), _matrixID));
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
	glm::vec3 _worldPosition(worldPosition);

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				blocks[x][y][z]->Render(_worldPosition);
				_worldPosition.z++;
			}
			_worldPosition.z = worldPosition.z;
			_worldPosition.y++;
		}
		_worldPosition.y = worldPosition.y;
		_worldPosition.x++;
	}
}