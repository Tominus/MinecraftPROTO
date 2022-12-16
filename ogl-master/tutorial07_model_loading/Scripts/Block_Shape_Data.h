#pragma once
#include <vector>
#include <glm/glm.hpp>

/*Stock vertexs and uvs.*/
struct SBlock_Shape_Data
{
	friend class Blocks_Global_Shapes;

private:
	SBlock_Shape_Data() = delete;
	//Block_Shape_Data(const std::vector<glm::vec3>* _vertexs, const std::vector<glm::vec2>* _uvs);
	SBlock_Shape_Data(const glm::vec3 _vertexs[], const glm::vec2 _uvs[], const size_t& _vertexsSize, const size_t& _uvsSize);
	~SBlock_Shape_Data();

public:
	//inline const std::vector<glm::vec3>* GetVertexs() const { return vertexs; }
	//inline const std::vector<glm::vec2>* GetUVs() const { return uvs; }
	inline const glm::vec3* GetVertexs() const { return vertexs; }
	inline const glm::vec2* GetUVs() const { return uvs; }
	inline const size_t& GetVertexsSize() const	{ return vertexsSize; }
	inline const size_t& GetUVsSize() const	{ return uvsSize; }

private:
	//const std::vector<glm::vec3>* vertexs;
	//const std::vector<glm::vec2>* uvs;

	const glm::vec3* vertexs;
	const glm::vec2* uvs;

	size_t vertexsSize;
	size_t uvsSize;
};