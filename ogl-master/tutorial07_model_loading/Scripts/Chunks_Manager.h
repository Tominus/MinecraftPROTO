#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Windows.h>

#include "GlobalDefine.h"
#include "Action.h"

class Chunks_Manager;
class Chunk;
class Thread;
class Thread_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

typedef struct SThread_AddChunk
{
	Thread* thisThread;
	Chunks_Manager* thisPtr;
	const glm::vec3* position;
} SThread_AddChunk, *SThread_AddChunk_Ptr;

class Chunks_Manager
{
	friend class World;
	friend class Chunk_Data_Generator;//debug

private:
	Chunks_Manager();
	~Chunks_Manager();

private:
	static void WINAPI AddChunk(SThread_AddChunk_Ptr _data);
	
	void AddStartingWorldBaseChunk();

	void UpdateChunksManager() const;
	void TickChunksManager() const;

	void UpdateRender();

	void CheckGenerateNewChunkRender();
	void CheckGenerateChunkPosition();

	void CheckRenderDistance();
	bool CheckIfNoChunkLoaded(const size_t& _worldChunkSize, const glm::vec3& _playerPositionChunkRelative);

	void DeleteChunksOutOfRange(std::vector<Chunk*>& _chunkInRange, size_t _worldChunkSize);

public:
	Chunk* GetChunkAtPosition(const glm::vec3& _position) const;
	bool GetIsChunkAtPositionBeingGenerated(const glm::vec3& _position) const;

	inline Chunk_Data_Generator* GetChunkDataGenerator() const { return chunkDataGenerator; }
	inline Chunk_Render_Generator* GetChunkRenderGenerator() const { return chunkRenderGenerator; }

private:
	std::vector<Chunk*> worldChunks;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;
	Thread_Manager* threadManager;

	std::vector<Chunk*> chunkWaitingForCGgen;
	std::vector<glm::vec3> chunkPositionBeingGenerated;
	std::vector<glm::vec3> chunkPositionFinishGeneration;

	Action<> onUpdate;
	Action<> onTick;

	mutable HANDLE mutex;

	int renderDistanceIndex;
	int renderDistance;
	int renderMaxDistance;
};