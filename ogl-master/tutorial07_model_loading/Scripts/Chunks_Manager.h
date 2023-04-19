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

public:
	inline Chunk* GetChunkAtPosition(const glm::vec3& _position) const
	{
		WaitForSingleObject(mutex, INFINITE);

		const size_t& _max = worldChunks.size();
		for (size_t i = 0; i < _max; ++i)
		{
			Chunk* _chunk = worldChunks[i];
			if (_position == _chunk->chunkPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		ReleaseMutex(mutex);
		return nullptr;
	}

	inline Chunk* GetFirstChunkAroundPosition(const glm::vec3& _position) const
	{
		const glm::vec3& _downPosition =  _position + glm::vec3(0.f, -1.f, 0.f);
		const glm::vec3& _upPosition =    _position + glm::vec3(0.f, 1.f, 0.f);
		const glm::vec3& _leftPosition =  _position + glm::vec3(-1.f, 0.f, 0.f);
		const glm::vec3& _rightPosition = _position + glm::vec3(1.f, 0.f, 0.f);
		const glm::vec3& _backPosition =  _position + glm::vec3(0.f, 0.f, -1.f);
		const glm::vec3& _frontPosition = _position + glm::vec3(0.f, 0.f, 1.f);

		WaitForSingleObject(mutex, INFINITE);

		const size_t& _max = worldChunks.size();
		for (size_t i = 0; i < _max; ++i)
		{
			Chunk* _chunk = worldChunks[i];
			const glm::vec3& _chunkPosition = _chunk->chunkPosition;
			
			if (_chunkPosition == _downPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
			if (_chunkPosition == _upPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
			if (_chunkPosition == _leftPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
			if (_chunkPosition == _rightPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
			if (_chunkPosition == _backPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
			if (_chunkPosition == _frontPosition)
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		ReleaseMutex(mutex);
		return nullptr;
	}

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