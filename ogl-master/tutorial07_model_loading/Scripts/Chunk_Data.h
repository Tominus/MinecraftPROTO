#pragma once
#include "Action.h"

#include <glm/glm.hpp>
#include <vector>
#include <Windows.h>

class Block;
class Chunk;

class Chunk_Data
{
	friend class Chunk;
	friend class Chunk_Data_Generator;
	friend class Chunk_Render_Generator;
	friend class Chunks_Manager;
	friend class Chunk_Pool_Manager;

private:
	Chunk_Data(Chunk* _ownerChunk, Chunk_Pool_Manager* _chunkPoolManager);
	~Chunk_Data();

	void AddSideChunk(Chunk* _chunk);
	void AddOtherSideChunk(Chunk_Data* _otherChunkData, const glm::vec3& _ownerPosition);
	void AddGeneratedSideChunk(const glm::vec3& _otherPosition);

	void RemoveSideChunk(Chunk* _chunk);

	bool CheckChunkToWaitEmpty();

	void PrePool();

public:
	inline Chunk* GetOwnerChunk() const { return ownerChunk; }

	inline Chunk* GetDownChunk()  const { return downChunk; }
	inline Chunk* GetUpChunk()    const { return upChunk; }
	inline Chunk* GetLeftChunk()  const { return leftChunk; }
	inline Chunk* GetRightChunk() const { return rightChunk; }
	inline Chunk* GetBackChunk()  const { return backChunk; }
	inline Chunk* GetFrontChunk() const { return frontChunk; }

	inline Block* GetBlock(const glm::uvec3& _position) const { return blocks[_position.x][_position.y][_position.z]; }

private:
	Block**** blocks;

	Chunk_Pool_Manager* chunkPoolManager;
	Chunks_Manager* chunkManager;
	HANDLE mutex_ChunkManager;
	Chunk* ownerChunk;

	std::vector<glm::vec3> chunkPositionToWait;
	bool bHasFinishWaitSideChunk;

	Chunk* downChunk;
	Chunk* upChunk;
	Chunk* leftChunk;
	Chunk* rightChunk;
	Chunk* backChunk;
	Chunk* frontChunk;
};