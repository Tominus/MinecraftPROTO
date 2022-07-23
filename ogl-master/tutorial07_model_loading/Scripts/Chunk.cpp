#include "Chunk.h"

#include "GlobalDefine.h"
#include "Chunk_Data.h"
#include "Chunk_render.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"

Chunk::Chunk(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator, const glm::vec3& _position)
{
	chunkPosition = _position;
	worldPosition = _position * 16.0f;

	chunkDataGenerator = _chunkDataGenerator;
	chunkRenderGenerator = _chunkRenderGenerator;

	chunkData = new Chunk_Data();
	chunkDataGenerator->GenerateNewChunkData(chunkData);

	chunkRender = new Chunk_Render(this);
	chunkRenderGenerator->GenerateNewChunkRender(chunkRender, chunkData);
}

Chunk::~Chunk()
{
	delete chunkData;
	delete chunkRender;
}

void Chunk::Render() const
{
	chunkRender->Render();
}

const glm::vec3& Chunk::GetChunkPosition() const
{
	return chunkPosition;
}

const glm::vec3& Chunk::GetWorldPosition() const
{
	return worldPosition;
}
