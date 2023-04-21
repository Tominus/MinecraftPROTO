#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Chunk_Render;
class Chunk_Data;
class Chunk_SideData;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

class Chunk
{
	friend class Chunk_Data;
	friend class Chunks_Manager;
	friend class Chunk_Render;
	friend class Chunk_Data_Generator;
	friend class Chunk_Render_Generator;

private:
	Chunk(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator, const glm::vec3& _position);
	~Chunk();

	void Init();

	void InitChunkData();
	void InitChunkRender();
	void FinishInit();

	void Render() const;
	void GenerateCGRender();
	void UpdateChunkSideRender();

	void PreDeleteChunk();

public:
	/*Return the position of the chunk.*/
	inline const glm::vec3& GetChunkPosition() const { return chunkPosition; }

	/*Return the chunkPosition * 16.f to have his position in the world.*/
	inline const glm::vec3& GetWorldPosition() const { return worldPosition; }

	inline Chunk_Data* GetChunkData() const { return chunkData; }

private:
	Chunk_Data* chunkData;
	Chunk_Render* chunkRender;
	Chunk_SideData* chunkSideData;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;

	glm::vec3 chunkPosition;
	glm::vec3 worldPosition;
};