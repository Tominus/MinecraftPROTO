#include "Block.h"

Block::Block(const EBlock_Type& _blockType)
{
	blockType = _blockType;
}

Block::~Block()
{

}

const EBlock_Type& Block::GetBlockType() const
{
	return blockType;
}