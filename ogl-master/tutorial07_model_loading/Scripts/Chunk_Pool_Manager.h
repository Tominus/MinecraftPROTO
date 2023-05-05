#pragma once
#include <vector>
#include <Windows.h>

class Chunk;
class Chunk_Data;
class Chunk_Render;
class Chunk_SideData;
class Chunk_Data_Generator;
class Chunk_Render_Generator;
class Chunks_Manager;

class Chunk_Pool_Manager
{

	friend class Chunks_Manager;

private:
	Chunk_Pool_Manager() = delete;
	Chunk_Pool_Manager(Chunks_Manager* _chunkManager, Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator);
	~Chunk_Pool_Manager();

	void InitializeAllChunkData();
	void DestroyAllChunkData();

	void GenerateChunkHandle(Chunk* _chunk);
	void GenerateChunkData(Chunk_Data* _chunkData);
	void GenerateChunkRender(Chunk_Render* _chunkRender);
	void GenerateChunkSideData(Chunk_SideData* _chunkSideData);

	void ClearChunkHandle(Chunk* _chunk);
	void ClearChunkData(Chunk_Data* _chunkData);
	void ClearChunkRender(Chunk_Render* _chunkRender);
	void ClearChunkSideData(Chunk_SideData* _chunkSideData);
	
	inline Chunk* GetChunk()
	{
		Chunk* _chunk = nullptr;

		WaitForSingleObject(mutex_ChunkPool, INFINITE);
		if (chunkPool.size() > 0)
		{
			_chunk = chunkPool.back();
			chunkPool.pop_back();
		}
		ReleaseMutex(mutex_ChunkPool);

		return _chunk;
	}

	inline void RetreiveChunk(Chunk* _chunk)
	{
		ClearChunkHandle(_chunk);
		ClearChunkData(_chunk->chunkData);
		ClearChunkRender(_chunk->chunkRender);
		ClearChunkSideData(_chunk->chunkSideData);

		WaitForSingleObject(mutex_ChunkPool, INFINITE);
		chunkPool.push_back(_chunk);
		ReleaseMutex(mutex_ChunkPool);
	}

private:
	std::vector<Chunk*> chunkPool;

	Chunks_Manager* chunkManager;
	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;

	HANDLE mutex_ChunkPool;
};