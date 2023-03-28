#pragma once
#include "Action.h"

#include <glm/glm.hpp>
#include <vector>

class Block;
class Chunk;

class Chunk_Data
{
	friend class Chunk;
	friend class Chunk_Data_Generator;
	friend class Chunk_Render_Generator;
	friend class Chunks_Manager;

private:
	Chunk_Data(Chunk* _ownerChunk);
	~Chunk_Data();

	void AddSideChunk(Chunk* _chunk);
	void AddOtherSideChunk(Chunk_Data*& _otherChunkData, const glm::vec3& _ownerPosition);

	void RemoveSideChunk(Chunk* _chunk);

	bool CheckChunkToWaitEmpty();

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

	Chunks_Manager* chunkManager;
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