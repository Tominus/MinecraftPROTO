#include "Block_Shape_Data.h"
#include "GlobalDefine.h"


SBlock_Shape_Data::SBlock_Shape_Data(const glm::vec3 _vertexs[], const glm::vec2 _uvs[], const size_t& _vertexsSize, const size_t& _uvsSize)
{
	vertexs = _vertexs;
	uvs = _uvs;
	vertexsSize = _vertexsSize;
	uvsSize = _uvsSize;
}

SBlock_Shape_Data::~SBlock_Shape_Data()
{
	delete[] vertexs;
	delete[] uvs;
}