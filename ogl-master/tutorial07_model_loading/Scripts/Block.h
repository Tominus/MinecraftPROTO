#pragma once
#include "Block_Type.h"

class Block
{
	friend class Chunk_Data; // for the delete*
	friend class Chunk_Data_Generator;
	friend class Chunk_SideData;
	friend class Chunks_Manager;

private:
	Block(const EBlock_Type& _blockType);
	~Block();

public:
	const EBlock_Type& GetBlockType() const;

private:
	EBlock_Type blockType;

};