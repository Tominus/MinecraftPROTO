#pragma once
#include <map>

#include "Block_Shapes_Type.h"
#include "Block_Shape_Data.h"

/*Stock every vertexs and uvs of all existing blocks.*/
class Blocks_Global_Shapes
{
	friend class World;

private:
	Blocks_Global_Shapes();
	~Blocks_Global_Shapes();

	void GenerateBlocksShapeDatas();

public:
	const Block_Shape_Data* GetBlockShapeData(const Block_Shapes_Type& _blockShapesType) const;

private:
	std::map<Block_Shapes_Type, Block_Shape_Data*> blocksShapes;
};