#pragma once
#include "Block_Type.h"

class Block
{
	friend class Chunk_Data; // for the delete*
	friend class Chunk_Data_Generator;

private:
	Block(const Block_Type& _blockType);
	~Block();

public:
	const Block_Type& GetBlockType() const;

private:
	Block_Type blockType;

};