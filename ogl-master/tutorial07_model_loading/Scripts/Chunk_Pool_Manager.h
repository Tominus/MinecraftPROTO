#pragma once
#include <vector>

class Chunk;
class Chunk_Data;
class Chunk_Render;
class Chunk_SideData;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

class Chunk_Pool_Manager
{

	friend class Chunks_Manager;

private:
	Chunk_Pool_Manager() = delete;
	Chunk_Pool_Manager(Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator);
	~Chunk_Pool_Manager();

	void InitializeAllChunkData();
	void DestroyAllChunkData();

private:
	std::vector<Chunk*> chunkPool;
	std::vector<Chunk_Data*> chunkDataPool;
	std::vector<Chunk_Render*> chunkRenderPool;
	std::vector<Chunk_SideData*> chunkSideDataPool;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;
};