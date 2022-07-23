#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"

Chunks_Manager::Chunks_Manager()
{
	chunkDataGenerator = new Chunk_Data_Generator();
	chunkRenderGenerator = new Chunk_Render_Generator(this);
}

Chunks_Manager::~Chunks_Manager()
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; i++)
	{
		delete worldChunks[i];
	}
	delete chunkDataGenerator;
	delete chunkRenderGenerator;
}

void Chunks_Manager::AddChunk(const glm::vec3& _position)
{
	worldChunks.push_back(new Chunk(chunkDataGenerator, chunkRenderGenerator, _position));
}

Chunk* Chunks_Manager::GetChunkAtPosition(const glm::vec3& _position) const
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; i++)
	{
		Chunk* _chunk = worldChunks[i];
		if (_position == _chunk->chunkPosition)
			return _chunk;
	}
	return nullptr;
}

void Chunks_Manager::Render() const
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; i++)
	{
		worldChunks[i]->Render();
	}
}