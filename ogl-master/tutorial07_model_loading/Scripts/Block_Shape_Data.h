#pragma once
#include <vector>
#include <glm/glm.hpp>

/*Stock vertexs and uvs.*/
struct Block_Shape_Data
{
	friend class Blocks_Global_Shapes;

private:
	Block_Shape_Data(const std::vector<glm::vec3>* _vertexs, const std::vector<glm::vec2>* _uvs);
	~Block_Shape_Data();

public:
	inline const std::vector<glm::vec3>* GetVertexs() const
	{
		return vertexs;
	}
	inline const std::vector<glm::vec2>* GetUVs() const
	{
		return uvs;
	}
	inline const size_t& GetVertexsSize() const
	{
		return vertexsSize;
	}
	inline const size_t& GetUVsSize() const
	{
		return uvsSize;
	}

private:
	const std::vector<glm::vec3>* vertexs;
	const std::vector<glm::vec2>* uvs;

	size_t vertexsSize;
	size_t uvsSize;
};