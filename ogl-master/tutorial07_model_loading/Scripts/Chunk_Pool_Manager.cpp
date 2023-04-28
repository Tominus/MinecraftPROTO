#include "Chunk_Pool_Manager.h"
#include "GlobalDefine.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Chunk_SideData.h"

Chunk_Pool_Manager::Chunk_Pool_Manager(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator)
{
	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;
}

Chunk_Pool_Manager::~Chunk_Pool_Manager()
{

}

void Chunk_Pool_Manager::InitializeAllChunkData()
{
	const size_t& _size = Render_Distance_Total * Render_Distance_Total * Chunk_Max_World_Height;
	for (size_t i = 0; i < _size; ++i)
	{
		Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator);

		Chunk_Data* _chunkData = new Chunk_Data(_chunk);
		Chunk_Render* _chunkRender = new Chunk_Render(_chunk);
		Chunk_SideData* _chunkSideData = new Chunk_SideData();





		_chunk->chunkData = _chunkData;
		_chunk->chunkRender = _chunkRender;
		_chunk->chunkSideData = _chunkSideData;

		chunkPool.push_back(_chunk);
	}
}

void Chunk_Pool_Manager::DestroyAllChunkData()
{

}