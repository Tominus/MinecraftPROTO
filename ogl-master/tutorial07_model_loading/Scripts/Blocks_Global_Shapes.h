#pragma once
#include <map>

#include "Block_Shapes_Type.h"
#include "Block_Shape_Data.h"

/*Stock every vertexs and uvs of all existing blocks.*/
class Blocks_Global_Shapes
{
	friend class World;
	friend class Chunk_Render_Generator;//

private:
	Blocks_Global_Shapes();
	~Blocks_Global_Shapes();

	void GenerateBlocksShapeDatas();

public:
	const SBlock_Shape_Data* GetBlockShapeData(const EBlock_Shapes_Type& _blockShapesType) const;

private:
	std::map<EBlock_Shapes_Type, SBlock_Shape_Data*> blocksShapes;
};