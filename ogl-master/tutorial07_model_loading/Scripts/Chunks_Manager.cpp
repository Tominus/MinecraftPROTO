#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"

Chunks_Manager::Chunks_Manager()
{
	chunkDataGenerator = new Chunk_Data_Generator();
	chunkRenderGenerator = new Chunk_Render_Generator(this);

	onUpdate = [this]()
	{
		CheckForNewChunk();
		UpdateRender();
	};
}

Chunks_Manager::~Chunks_Manager()
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		delete worldChunks[i];
	}

	const size_t& _max2 = chunkWaitingForCGgen.size();
	for (size_t i = 0; i < _max2; ++i)
	{
		delete chunkWaitingForCGgen[i];
	}

	delete chunkDataGenerator;
	delete chunkRenderGenerator;
}

void Chunks_Manager::AddChunk(const glm::vec3& _position)
{
	mutex_chunkWaitingForCGgen.lock();
	Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _position);
	chunkWaitingForCGgen.push_back(_chunk);
	mutex_chunkWaitingForCGgen.unlock();
}

Chunk* Chunks_Manager::GetChunkAtPosition(const glm::vec3& _position) const
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk* _chunk = worldChunks[i];
		if (_position == _chunk->chunkPosition)
			return _chunk;
	}
	return nullptr;
}

void Chunks_Manager::UpdateChunksManager() const
{
	std::invoke(onUpdate);
}

void Chunks_Manager::CheckForNewChunk()
{
	mutex_chunkWaitingForCGgen.lock();
	unsigned int _size = chunkWaitingForCGgen.size();
	while (_size > 0)
	{
		Chunk* _chunk = chunkWaitingForCGgen[0];
		_chunk->GenerateCGRender();

		chunkWaitingForCGgen.erase(chunkWaitingForCGgen.begin());
		--_size;

		worldChunks.push_back(_chunk);
	}
	mutex_chunkWaitingForCGgen.unlock();
}

void Chunks_Manager::UpdateRender()
{
	const size_t& _max = worldChunks.size();

	for (size_t i(0); i < _max; ++i)
	{
		worldChunks[i]->Render();
	}
	
	if (_max > 10)
	{
		for (size_t i = 0; i < 1; i++)
		{
			delete worldChunks[i];
		}
		worldChunks.erase(worldChunks.begin(), worldChunks.begin() + 1);
	}
}