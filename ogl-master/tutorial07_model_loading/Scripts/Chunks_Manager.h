#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Windows.h>

#include "GlobalDefine.h"
#include "Action.h"
#include "Chunk.h"

#include "Thread_Structs.h"

class Chunks_Manager;
class Chunk;
class Thread;
class Thread_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;
class Blocks_Global_Datas;
class Chunk_Pool_Manager;

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
	void InitWorldChunksArray();
	void InitChunksClear();

	static int WINAPI GenerateChunk(SThread_AddChunk* _data);
	static int WINAPI Opti_GenerateChunk(SThread_AddChunk* _data);
	static int WINAPI ClearChunks(SThread_ClearChunks* _data);
	
	void AddStartingWorldBaseChunk();
	void AddWaitingForSideUpdateChunk(Chunk* _chunk);

	void UpdateChunksManager() const;
	void TickChunksManager() const;

	void UpdateRender();

	void Exit();
	void PostExitUpdate();

	void CheckGenerateNewChunkRender();
	void CheckGenerateChunkPosition();
	void CheckUpdateChunkSideRender();

	void Opti_CheckRenderDistance();
	void Opti_RecenterWorldChunksArray(const glm::vec3& _playerPositionChunkRelative);
	bool Opti_CheckIfNoChunkLoaded(glm::vec3 _playerPositionChunkRelative);
	

public:
	// Return if the chunk have been added to array
	inline bool Opti_AddChunk(Chunk* _chunk) 
	{
		const glm::ivec3& _offset = _chunk->chunkPosition - opti_worldChunksOffset;
		const int& _x = _offset.x;
		const int& _z = _offset.z;

		if (_x < 0 || _x >= Render_Distance_Total || _z < 0 || _z >= Render_Distance_Total)
		{
			return false;
		}

		opti_worldChunks[_x][_offset.y][_z] = _chunk;
		++opti_worldChunksCount;

		return true;
	}

	// Return the chunk at position if existing
	inline Chunk* Opti_GetChunk(const glm::vec3& _position)
	{
		Chunk* _chunk = nullptr;

		WaitForSingleObject(mutex, INFINITE);
		const glm::ivec3& _offset = _position - opti_worldChunksOffset;
		const int& _x = _offset.x;
		const int& _z = _offset.z;

		if (_x > -1 && _x < Render_Distance_Total && _z > -1 && _z < Render_Distance_Total)
		{
			_chunk = opti_worldChunks[_x][_offset.y][_z];
		}

		ReleaseMutex(mutex);
		return _chunk;
	}

	// Return the first encounter chunk around position
	inline Chunk* Opti_GetFirstChunkAroundPosition(const glm::vec3& _position)
	{
		WaitForSingleObject(mutex, INFINITE);
		const glm::ivec3& _offset = _position - opti_worldChunksOffset;
		const int& _x = _offset.x;
		const int& _y = _offset.y;
		const int& _z = _offset.z;

		if (_x > -2 && _x <= Render_Distance_Total && _z > -2 && _z <= Render_Distance_Total)
		{
			if (_x > 0)
			{
				if (Chunk* _chunk = opti_worldChunks[_x - 1][_y][_z])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
			}
			if (_y > 0)
			{
				if (Chunk* _chunk = opti_worldChunks[_x][_y - 1][_z])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
			}
			if (_z > 0)
			{
				if (Chunk* _chunk = opti_worldChunks[_x][_y][_z - 1])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
			}
			if (_x < Render_Distance_Total_Limit)
			{
				if (Chunk* _chunk = opti_worldChunks[_x + 1][_y][_z])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
			}
			if (_y < Chunk_Max_Limit_World_Height)
			{
				if (Chunk* _chunk = opti_worldChunks[_x][_y + 1][_z])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
			}
			if (_z < Render_Distance_Total_Limit)
			{
				if (Chunk* _chunk = opti_worldChunks[_x][_y][_z + 1])
				{
					ReleaseMutex(mutex);
					return _chunk;
				}
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
	Chunk**** opti_worldChunks;
	size_t opti_worldChunksCount;
	glm::vec3 opti_worldChunksOffset;

	int opti_threadCount;

	std::vector<Chunk*> worldChunksToRender;
	std::vector<Chunk*> worldChunksToClear;

	Chunk_Pool_Manager* chunkPoolManager;
	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;
	Thread_Manager* threadManager;
	World* world;

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
	mutable HANDLE mutex_ChunksToClear;

	GLuint programID;

	bool bInterruptThread_NotSafe;

	int renderDistance;
	int renderMaxDistance;
};