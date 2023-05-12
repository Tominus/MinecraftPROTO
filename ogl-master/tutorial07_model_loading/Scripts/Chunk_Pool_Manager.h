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

struct SChunk_Render_Buffer;
struct SChunk_Render_Data;
struct SChunk_Render_Shapes;

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
	void GenerateChunkSideData();
	void GenerateChunkRenderBuffer();
	void GenerateChunkRenderData();
	void GenerateChunkRenderShapes();

	void ClearChunkHandle(Chunk* _chunk);
	void ClearChunkData(Chunk_Data* _chunkData);
	void ClearChunkRender(Chunk_Render* _chunkRender);
	void ClearChunkSideData(Chunk_SideData* _chunkSideData);

	void Update();
	void CheckHasToGenerateNewChunkRenderBuffer();
	void CheckHasToGenerateNewChunkRenderData();
	void CheckHasToGenerateNewChunkRenderShapes();

public:
	void SetHasToGenerateNewChunkRenderBuffer();
	void SetHasToGenerateNewChunkRenderData();
	void SetHasToGenerateNewChunkRenderShapes();
	
public:
	Chunk* GetChunk();
	void RetreiveChunk(Chunk* _chunk);

	Chunk_SideData* GetChunkSideData();
	void RetreiveChunkSideData(Chunk_SideData* _chunkSideData);

	SChunk_Render_Buffer* GetChunkRenderBuffer();
	void RetreiveChunkRenderBuffer(SChunk_Render_Buffer* _chunkRenderBuffer);

	SChunk_Render_Data* GetChunkRenderData();
	void RetreiveChunkRenderData(SChunk_Render_Data* _chunkRenderData);

	SChunk_Render_Shapes* GetChunkRenderShapes();
	void RetreiveChunkRenderShapes(SChunk_Render_Shapes* _chunkRenderShapes);

private:
	std::vector<Chunk*> chunkPool;
	std::vector<Chunk*> debug;
	std::vector<Chunk_SideData*> chunkSideDataPool;
	std::vector<SChunk_Render_Buffer*> chunkRenderBufferPool;
	std::vector<SChunk_Render_Data*> chunkRenderDataPool;
	std::vector<SChunk_Render_Shapes*> chunkRenderShapesPool;

	Chunks_Manager* chunkManager;
	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;

	HANDLE mutex_ChunkPool;
	HANDLE mutex_ChunkSideDataPool;
	HANDLE mutex_ChunkRenderBufferPool;
	HANDLE mutex_ChunkRenderDataPool;
	HANDLE mutex_ChunkRenderShapesPool;
	HANDLE mutex_HasToGenerateData;

	bool bHasToGenerateNewChunkRenderBuffer;
	bool bHasToGenerateNewChunkRenderData;
	bool bHasToGenerateNewChunkRenderShapes;

	int iTotalBuffer;
	int iTotalData;
	int iTotalShapes;
};