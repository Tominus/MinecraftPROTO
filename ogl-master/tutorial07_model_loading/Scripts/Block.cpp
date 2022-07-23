#include "Block.h"

Block::Block(const Block_Type& _blockType)
{
	blockType = _blockType;
}

Block::~Block()
{

}

const Block_Type& Block::GetBlockType() const
{
	return blockType;
}