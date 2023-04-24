#include "Blocks_Global_Datas.h"
#include "World.h"
#include "Blocks_Global_Shapes.h"

#define BLOCK_DATAS(_type, _render, _shapes) EBlock_Type::_type, EBlock_Render_Type::_render, EBlock_Shapes_Type::_shapes

Blocks_Global_Datas::Blocks_Global_Datas()
{
	blockDatas = nullptr;
	blockDatasSize = (size_t)EBlock_Type::BLOCK_TYPE_MAX_NUMBER;

	World* _world = World::Instance();
	blocksGlobalShapes = _world->GetBlocksGlobalShapes();
}

Blocks_Global_Datas::~Blocks_Global_Datas()
{
	delete[] blockDatas;
}

const SBlock_Datas const* Blocks_Global_Datas::GetBlockData(const EBlock_Type& _blockType) const
{
	return &blockDatas[(size_t)_blockType];
}

void Blocks_Global_Datas::GenerateBlocksGlobalDatas()
{
	blockDatas = new SBlock_Datas[blockDatasSize];

	blockDatas[0].SetBlockDatas(BLOCK_DATAS(Air, No_Render, Block));
	blockDatas[1].SetBlockDatas(BLOCK_DATAS(Acacia_Log, Opaque, Block));
	blockDatas[2].SetBlockDatas(BLOCK_DATAS(Amethyst_Block, Opaque, Block));
	blockDatas[3].SetBlockDatas(BLOCK_DATAS(Barrel_Side, Opaque, Block));
	blockDatas[4].SetBlockDatas(BLOCK_DATAS(Bee_Nest_Top, Opaque, Block));
	blockDatas[5].SetBlockDatas(BLOCK_DATAS(Deepslate_Diamond_Ore, Opaque, Block));
	blockDatas[6].SetBlockDatas(BLOCK_DATAS(Glass, Transparent, Block));
	blockDatas[7].SetBlockDatas(BLOCK_DATAS(Ice, Transparent, Block));
	blockDatas[8].SetBlockDatas(BLOCK_DATAS(Lime_Stained_Glass, Transparent, Block));
	blockDatas[9].SetBlockDatas(BLOCK_DATAS(Red_Stained_Glass, Transparent, Block));
}