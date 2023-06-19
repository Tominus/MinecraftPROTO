#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <Windows.h>
#include <vector>

class Thread;
class Chunks_Manager;
class Chunk;
class Chunk_Data_Generator;
class Chunk_Render_Generator;
class Chunk_Pool_Manager;

typedef struct SThread_AddChunk
{
	Thread* thisThread = nullptr;
	Chunks_Manager* chunkManager = nullptr;
	Chunk* chunk = nullptr;
	glm::vec3 playerPositionChunkRelative;

	SThread_AddChunk() = default;

	SThread_AddChunk(Thread* _thisThread, Chunks_Manager* _chunkManager, Chunk* _chunk, const glm::vec3& _playerPositionChunkRelative)
	{
		thisThread = _thisThread;
		chunkManager = _chunkManager;
		chunk = _chunk;
		playerPositionChunkRelative = _playerPositionChunkRelative;
	}

} SThread_AddChunk, *SThread_AddChunk_Ptr;

typedef struct SThread_ClearChunks
{
	Thread* thisThread = nullptr;
	bool* bExitWorld = nullptr;
	std::vector<Chunk*>* worldChunksToClear = nullptr;
	HANDLE mutex_ChunksToClear;
	Chunk_Pool_Manager* chunkPoolManager = nullptr;

	SThread_ClearChunks() = default;

	SThread_ClearChunks(Thread* _thisThread, bool* _bExitWorld, std::vector<Chunk*>* _worldChunksToClear,
		                HANDLE _mutex_ChunksToClear, Chunk_Pool_Manager* _chunkPoolManager)
	{
		thisThread = _thisThread;
		bExitWorld = _bExitWorld;
		worldChunksToClear = _worldChunksToClear;
		mutex_ChunksToClear = _mutex_ChunksToClear;
		chunkPoolManager = _chunkPoolManager;
	}

} SThread_ClearChunks, *SThread_ClearChunks_Ptr;