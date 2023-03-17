#pragma once
#include "Block_Type.h"
#include "Block_Render_Type.h"
#include "Block_Shapes_Type.h"

struct SBlock_Datas
{
	friend class Blocks_Global_Datas;

private:
	SBlock_Datas() = default;

	void SetBlockDatas(const EBlock_Type& _blockType, const EBlock_Render_Type& _blockRenderType, const EBlock_Shapes_Type& _blockShapeType)
	{
		blockType = _blockType;
		blockRenderType = _blockRenderType;
		blockShapeType = _blockShapeType;
	}

public:
	EBlock_Type blockType;

	EBlock_Render_Type blockRenderType;
	EBlock_Shapes_Type blockShapeType;
};