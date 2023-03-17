#pragma once
#include "Block_Datas.h"

class Blocks_Global_Shapes;

class Blocks_Global_Datas
{

	friend class World;

private:
	Blocks_Global_Datas();
	~Blocks_Global_Datas();

public:
	const SBlock_Datas const* GetBlockData(const EBlock_Type& _blockType) const;

private:
	void GenerateBlocksGlobalDatas();

private:
	Blocks_Global_Shapes* blocksGlobalShapes;

	SBlock_Datas* blockDatas;
	size_t blockDatasSize;
};