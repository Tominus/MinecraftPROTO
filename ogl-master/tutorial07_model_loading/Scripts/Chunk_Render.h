#pragma once
#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

/*Rendu pour chaque block. Peut être nullptr.
Permet uniquement de retrouver le rendu d'un block en particulier.*/
struct Chunk_Render_Shapes
{
	const std::vector<glm::vec3>* vertexs;
	const std::vector<glm::vec2>* uvs;

	Chunk_Render_Shapes(const std::vector<glm::vec3>* _vertexs, const std::vector<glm::vec2>* _uvs)
	{
		vertexs = _vertexs;
		uvs = _uvs;
	}
};

/*Stock pour une texture tout les blocks qu'il va devoir rendre.*/
struct Chunk_Render_Data
{
	std::vector<const Chunk_Render_Shapes*> renderBuffer;
	unsigned int verticesGlobalSize;

	GLuint vertexsBuffer;
	GLuint uvsBuffer;

	//Used for the graphics generation
	std::vector<glm::vec3> globalVertexs;
	std::vector<glm::vec2> globalUVs;
};

class Chunk_Render
{
	friend class Chunk;
	friend class Chunk_Render_Generator;

private:
	Chunk_Render(Chunk* _ownerChunk);
	~Chunk_Render();

	void Render();

	/* Maybe used the position of the camera? and render progressively from nearest to farest.
	* Calculing with chunkPosition may be a good idea for the start.*/
	void RenderWithAlpha();

	inline void SetMatrixID(const GLuint& _matrixID)
	{
		matrixID = _matrixID;
	}

private:
	Chunk* ownerChunk;
	glm::vec3 ownerChunkPosition;

	GLuint matrixID;
	
	/*Stock les rendu dans leur world position.*/
	std::vector<std::vector<std::vector<Chunk_Render_Shapes*>>> allBlockShapes;

	/*Stock tout les datas à rendre. textures -> */
	std::map<GLuint, Chunk_Render_Data*> renderDatas;
};