#pragma once
#include <vector>

class Block;

class Chunk_Data
{
	friend class Chunk;
	friend class Chunk_Data_Generator;
	friend class Chunk_Render_Generator;

private:
	Chunk_Data();
	~Chunk_Data();

private:
	std::vector<std::vector<std::vector<Block*>>> blocks;
};