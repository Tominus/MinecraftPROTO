#pragma once
#include "Block_Type.h"

class Block
{
	friend class Chunk_Data;

private:
	Block(const Block_Type& _blockType);
	~Block();

public:
	const Block_Type& GetBlockType() const;

private:
	Block_Type blockType;

};