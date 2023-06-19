#include "Blocks_Global_Shapes.h"
#include "GlobalDefine.h"

Blocks_Global_Shapes::Blocks_Global_Shapes()
{
	
}

Blocks_Global_Shapes::~Blocks_Global_Shapes()
{
	for each (const std::pair<EBlock_Shapes_Type, SBlock_Shape_Data*>& _blockShapes in blocksShapes)
	{
		delete _blockShapes.second;
	}

	for each (const std::pair<EBlock_Shapes_Type, std::vector<SBlock_Shape_Data*>>& _blockShapesDatas in blockShapesDatas)
	{
		const std::vector<SBlock_Shape_Data*>& _blockShape = _blockShapesDatas.second;

		const size_t& _max = _blockShape.size();
		for (size_t i = 0; i < _max; ++i)
		{
			delete _blockShape[i];
		}
	}
}

void Blocks_Global_Shapes::GenerateBlocksShapeDatas()
{
	//Block
	{
		glm::vec3* _vertices = new glm::vec3[Block_Total_Shapes]{
			//D
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),

			//U
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),

			//L
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),

			//R
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),

			//B
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),

			//F
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 0.0f, 1.0f),
		glm::vec3(0.0f, 1.0f, 1.0f),
		glm::vec3(0.0f, 0.0f, 1.0f), 
	};
		glm::vec2* _uvs = new glm::vec2[Block_Total_Shapes]{
		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),


		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),

		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
	};
		const size_t* _vertexsSize = new size_t(Block_Total_Shapes);

		SBlock_Shape_Data* _datasBlock = new SBlock_Shape_Data(_vertices, _uvs, _vertexsSize, Block_Total_Shapes);
		blocksShapes.emplace(EBlock_Shapes_Type::Block, _datasBlock);

		std::vector<SBlock_Shape_Data*> _blockShapes;

		//There is no render for the first block
		_blockShapes.push_back(nullptr);

		for (size_t i = 1; i < Block_Total_Possible_Shapes; ++i)
		{
			std::vector<glm::vec3> _tmpVertices;
			std::vector<glm::vec2> _tmpUVs;

			for (size_t j = 1, index = 0; j < 64; j *= 2, index += 6)
			{
				//Binaries check 
				if (i & j)
				{
					//Create vertexs and uvs for each possible block render
					for (size_t k = 0; k < 6; ++k)
					{
						_tmpVertices.push_back(_vertices[index + k]);
						_tmpUVs.push_back(_uvs[index + k]);
					}
				}
			}

			const size_t& _max = _tmpVertices.size();
			const size_t* _shapeVertexsSize = new size_t(_max);

			glm::vec3* _shapeVertices = new glm::vec3[_max];
			glm::vec2* _shapeUVs = new glm::vec2[_max];

			for (size_t j = 0; j < _max; ++j)
			{
				_shapeVertices[j] = _tmpVertices[j];
				_shapeUVs[j] = _tmpUVs[j];
			}

			_blockShapes.push_back(new SBlock_Shape_Data(_shapeVertices, _shapeUVs,
														 _shapeVertexsSize, _max));
		}

		blockShapesDatas.emplace(EBlock_Shapes_Type::Block, _blockShapes);
	}

	//Something else...

}

const SBlock_Shape_Data* Blocks_Global_Shapes::GetBlockShapeData(const EBlock_Shapes_Type& _blockShapesType) const
{
	return blocksShapes.at(_blockShapesType);
}

const SBlock_Shape_Data* Blocks_Global_Shapes::GetBlockVertexsAndUVs(const EBlock_Shapes_Type& _blockShapesType, unsigned _index)
{
	return blockShapesDatas.at(_blockShapesType)[_index];
}

unsigned Blocks_Global_Shapes::GetVertexsIndex(const EBlock_Shapes_Type& _blockShapesType, const glm::vec3* _vertexs)
{
	std::vector<SBlock_Shape_Data*>& _data = blockShapesDatas.at(_blockShapesType);

	const size_t& _max = _data.size();

	//_data[0] is always nullptr
	for (size_t i = 1; i < _max; ++i)
	{
		if (_vertexs == _data[i]->vertexs)
			return i;
	}

	throw std::exception("[Blocks_Global_Shapes::GetVertexsIndex] -> This vertexs doesn't exist");
	return 999999999;
}