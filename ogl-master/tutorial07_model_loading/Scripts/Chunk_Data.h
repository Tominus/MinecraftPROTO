#pragma once

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

public:
	inline Chunk* GetOwnerChunk() const { return ownerChunk; }

	inline Chunk* GetDownChunk() const { return downChunk; }
	inline Chunk* GetUpChunk() const { return upChunk; }
	inline Chunk* GetLeftChunk() const { return leftChunk; }
	inline Chunk* GetRightChunk() const { return rightChunk; }
	inline Chunk* GetBackChunk() const { return backChunk; }
	inline Chunk* GetFrontChunk() const { return frontChunk; }

private:
	Block**** blocks;

	Chunk* ownerChunk;

	Chunk* downChunk;
	Chunk* upChunk;
	Chunk* leftChunk;
	Chunk* rightChunk;
	Chunk* backChunk;
	Chunk* frontChunk;
};