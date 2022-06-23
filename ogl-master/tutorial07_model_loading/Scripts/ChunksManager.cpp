#include "ChunksManager.h"
#include "Chunk.h"
#include "World.h"

ChunksManager::ChunksManager()
{

}

ChunksManager::~ChunksManager()
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; i++)
	{
		delete worldChunks[i];
	}
}

void ChunksManager::AddChunk(const glm::vec3& _position)
{
	const World* _world = &World::Instance();
	worldChunks.push_back(new Chunk(_world->GetMatrixID(), _position));
}

void ChunksManager::Render()
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; i++)
	{
		worldChunks[i]->Render_NormalBlock();
	}
	for (size_t i = 0; i < _max; i++)
	{
		worldChunks[i]->Render_AlphaBlock();
	}
}