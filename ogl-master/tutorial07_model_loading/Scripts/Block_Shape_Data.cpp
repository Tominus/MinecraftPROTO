#include "Block_Shape_Data.h"

Block_Shape_Data::Block_Shape_Data(const std::vector<glm::vec3>* _vertexs, const std::vector<glm::vec2>* _uvs)
{
	vertexs = _vertexs;
	vertexsSize = vertexs->size();
	uvs = _uvs;
	uvsSize = _uvs->size();
}

Block_Shape_Data::~Block_Shape_Data()
{
	delete vertexs;
	delete uvs;
}
