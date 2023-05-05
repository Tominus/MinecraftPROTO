#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

class Thread;
class Chunks_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

typedef struct SThread_AddChunk
{
	Thread* thisThread = nullptr;
	Chunks_Manager* chunkManager = nullptr;
	glm::vec3 playerPositionChunkRelative;

	SThread_AddChunk() = default;

	SThread_AddChunk(Thread* _thisThread, Chunks_Manager* _chunkManager, const glm::vec3& _playerPositionChunkRelative)
	{
		thisThread = _thisThread;
		chunkManager = _chunkManager;
		playerPositionChunkRelative = _playerPositionChunkRelative;
	}

} SThread_AddChunk, *SThread_AddChunk_Ptr;