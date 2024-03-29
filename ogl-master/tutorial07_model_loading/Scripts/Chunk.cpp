#include "Chunk.h"

#include "GlobalDefine.h"
#include "Chunk_Data.h"
#include "Chunk_render.h"
#include "Chunk_SideData.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"
#include "Thread_Structs.h"

Chunk::Chunk(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator)
{
	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;
	handle_AddChunk = nullptr;
	chunkData = nullptr;
	chunkRender = nullptr;
	chunkSideData = nullptr;
}

Chunk::Chunk(SThread_AddChunk* _data, Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator, const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * (float)Chunk_Size;

	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;

	chunkData = nullptr;
	chunkRender = nullptr;
	chunkSideData = nullptr;

	handle_AddChunk = _data;
}

Chunk::~Chunk()
{
	delete handle_AddChunk;
	delete chunkData;
	delete chunkRender;
	delete chunkSideData;
}

void Chunk::SetLocalPosition(const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * (float)Chunk_Size;
}

void Chunk::InitChunkData()
{
	chunkDataGenerator->GenerateNewChunkData(chunkData);
}

void Chunk::InitChunkRender()
{
	chunkRenderGenerator->GenerateNewChunkRender(chunkRender, chunkData);
}

void Chunk::Render() const
{
	chunkRender->Render();
}

void Chunk::GenerateCGRender()
{
	chunkRenderGenerator->GenerateChunkCGRender(chunkRender->renderDatas);
}

void Chunk::PrePoolChunk()
{
	chunkData->PrePool();
}