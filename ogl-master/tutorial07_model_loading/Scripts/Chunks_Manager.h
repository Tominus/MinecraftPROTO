#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Windows.h>

#include "GlobalDefine.h"
#include "Action.h"
#include "Chunk.h"

class Chunks_Manager;
class Chunk;
class Thread;
class Thread_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;
class Blocks_Global_Datas;

typedef struct SThread_AddChunk
{
	Thread* thisThread = nullptr;
	Chunks_Manager* thisPtr = nullptr;
	Chunk* chunk = nullptr;
} SThread_AddChunk, *SThread_AddChunk_Ptr;

class Chunks_Manager
{
	friend class World;
	friend class Chunk_Render_Generator;
	friend class Chunk_Data_Generator;
	friend class Chunk_Data;

private:
	Chunks_Manager();
	~Chunks_Manager();

private:
	void StartChunkManager();

	static void WINAPI AddChunk(SThread_AddChunk_Ptr _data);
	
	void AddStartingWorldBaseChunk();
	void AddWaitingForSideUpdateChunk(Chunk* _chunk);

	void UpdateChunksManager() const;
	void TickChunksManager() const;

	void UpdateRender();

	void Exit();

	void CheckGenerateNewChunkRender();
	void CheckGenerateChunkPosition();
	void CheckUpdateChunkSideRender();

	void CheckRenderDistance();
	bool CheckIfNoChunkLoaded(const size_t& _worldChunkSize, glm::vec3 _playerPositionChunkRelative);

	void DeleteChunksOutOfRange(std::vector<Chunk*>& _chunkInRange, size_t _worldChunkSize);

	void Opti_CheckRenderDistance();
	bool Opti_CheckIfNoChunkLoaded(glm::vec3 _playerPositionChunkRelative);
	

public:
	// Return if the chunk have been added to array
	bool Opti_AddChunk(Chunk* _chunk);

	// Return the chunk at position if existing
	Chunk* Opti_GetChunk(const glm::vec3& _position);

	// Return the first encouter chunk around position
	Chunk* Opti_GetFirstChunkAroundPosition(const glm::vec3& _position);


	inline bool GetIsChunkAtPositionBeingGenerated(const glm::vec3& _position) const
	{
		WaitForSingleObject(mutex, INFINITE);

		const size_t& _max = chunkPositionBeingGenerated.size();
		for (size_t i = 0; i < _max; ++i)
		{
			if (chunkPositionBeingGenerated[i] == _position)
			{
				ReleaseMutex(mutex);
				return true;
			}
		}
		ReleaseMutex(mutex);
		return false;
	}

	inline Chunk* GetChunkBeingGeneratedAtPosition(const glm::vec3& _position) const
	{
		WaitForSingleObject(mutex, INFINITE);

		const size_t& _max = chunkBeingGenerating.size();
		for (size_t i = 0; i < _max; ++i)
		{
			Chunk* _chunk = chunkBeingGenerating[i];
			if (_position == _chunk->chunkPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		ReleaseMutex(mutex);
		return nullptr;
	}

	inline Chunk_Data_Generator* GetChunkDataGenerator() const { return chunkDataGenerator; }
	inline Chunk_Render_Generator* GetChunkRenderGenerator() const { return chunkRenderGenerator; }

private:
	std::vector<Chunk*> worldChunks;

	Chunk**** opti_worldChunks;
	size_t opti_worldChunksCount;
	glm::vec3 opti_worldChunksOffset;

	std::vector<Chunk*> worldChunksToRender;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;
	Thread_Manager* threadManager;

	std::vector<Chunk*> chunkBeingGenerating;

	std::vector<Chunk*> chunkWaitingForCGgen;
	std::vector<Chunk*> chunkWaitingForGraphicalUpdate;

	std::vector<glm::vec3> chunkPositionBeingGenerated;
	std::vector<glm::vec3> chunkPositionFinishGeneration;

	Action<> onUpdate;
	Action<> onTick;
	Action<Chunk*> onChunkInitialized;
	Action<Chunk*> onChunkDestroyed;

	mutable HANDLE mutex;

	GLuint programID;

	bool bInterruptThread_NotSafe;

	int renderDistance;
	int renderMaxDistance;
};