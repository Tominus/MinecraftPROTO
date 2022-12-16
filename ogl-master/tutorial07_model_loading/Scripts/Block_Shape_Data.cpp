#include "Block_Shape_Data.h"

/*Block_Shape_Data::Block_Shape_Data(const std::vector<glm::vec3>* _vertexs, const std::vector<glm::vec2>* _uvs)
{
	vertexs = _vertexs;
	vertexsSize = vertexs->size();
	uvs = _uvs;
	uvsSize = uvs->size();
}*/

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