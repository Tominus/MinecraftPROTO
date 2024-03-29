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
	const SBlock_Shape_Data* GetBlockShapeData(const EBlock_Shapes_Type& _blockShapesType) const;

	const SBlock_Shape_Data* GetBlockVertexsAndUVs(const EBlock_Shapes_Type& _blockShapesType, unsigned _index);

	unsigned GetVertexsIndex(const EBlock_Shapes_Type& _blockShapesType, const glm::vec3* _vertexs);

private:
	std::map<EBlock_Shapes_Type, SBlock_Shape_Data*> blocksShapes;

	std::map<EBlock_Shapes_Type, std::vector<SBlock_Shape_Data*>> blockShapesDatas;
};