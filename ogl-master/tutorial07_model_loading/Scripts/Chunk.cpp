#include "Chunk.h"

#include "GlobalDefine.h"
#include "Chunk_Data.h"
#include "Chunk_render.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"

Chunk::Chunk(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator, const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * (float)Chunk_Size;

	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;

	chunkData = new Chunk_Data(this);
	chunkRender = new Chunk_Render(this);
}

Chunk::~Chunk()
{
	delete chunkData;
	delete chunkRender;
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

void Chunk::UpdateChunkSideRender()
{
	chunkRenderGenerator->UpdateChunkSideRender(this);
}