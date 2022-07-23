#pragma once

class Chunk_Data;

class Chunk_Data_Generator
{
	friend class Chunk;
	friend class Chunks_Manager;

private:
	Chunk_Data_Generator();
	~Chunk_Data_Generator();

	/*Generate all existing block randomly.*/
	void GenerateNewChunkData(Chunk_Data* _chunkData) const;

private:
	//maybe the generator seed and a ?Terrain_Generator
	unsigned randMax;
};