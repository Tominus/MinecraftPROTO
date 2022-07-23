#pragma once
#include <vector>
#include <glm/glm.hpp>

/*Stock vertexs and uvs.*/
struct Block_Shape_Data
{
	friend class Blocks_Global_Shapes;

private:
	Block_Shape_Data(const std::vector<glm::vec3>& _vertexs, const std::vector<glm::vec2>& _uvs)
	{
		vertexs = _vertexs;
		vertexsSize = _vertexs.size();
		uvs = _uvs;
		uvsSize = _uvs.size();
	}

public:
	const std::vector<glm::vec3>& GetVertexs() const
	{
		return vertexs;
	}
	const std::vector<glm::vec2>& GetUVs() const
	{
		return uvs;
	}
	const size_t& GetVertexsSize() const
	{
		return vertexsSize;
	}
	const size_t& GetUVsSize() const
	{
		return uvsSize;
	}

private:
	std::vector<glm::vec3> vertexs;
	std::vector<glm::vec2> uvs;

	size_t vertexsSize;
	size_t uvsSize;
};