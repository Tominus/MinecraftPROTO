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
	Chunks_Manager* thisPtr = nullptr;
	Chunk_Data_Generator* chunkDataGenerator = nullptr;
	Chunk_Render_Generator* chunkRenderGenerator = nullptr;
	glm::vec3 playerPositionChunkRelative;

	SThread_AddChunk() = default;
	SThread_AddChunk(Thread* _thisThread, Chunks_Manager* _thisPtr,
		Chunk_Data_Generator* _chunkDataGenerator, Chunk_Render_Generator* _chunkRenderGenerator,
		const glm::vec3& _playerPositionChunkRelative)
	{
		thisThread = _thisThread;
		thisPtr = _thisPtr;
		chunkDataGenerator = _chunkDataGenerator;
		chunkRenderGenerator = _chunkRenderGenerator;
		playerPositionChunkRelative = _playerPositionChunkRelative;
	}

} SThread_AddChunk, *SThread_AddChunk_Ptr;