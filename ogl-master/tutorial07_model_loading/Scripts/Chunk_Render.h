#pragma once
#include "Side.h"

#include <map>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "GlobalDefine.h"

/*Rendu pour chaque block. Peut être nullptr.
Permet uniquement de retrouver le rendu d'un block en particulier.*/
struct SChunk_Render_Shapes
{
	const glm::vec3* vertexs = nullptr;
	const glm::vec2* uvs = nullptr;
	const size_t* vertexsSize = nullptr;

	SChunk_Render_Shapes() = delete;
	SChunk_Render_Shapes(const glm::vec3 _vertexs[], const glm::vec2 _uvs[], const size_t* _vertexsSize)
	{
		vertexs = _vertexs;
		uvs = _uvs;
		vertexsSize = _vertexsSize;
	}
};

struct SChunk_Render_Buffer
{
	const SChunk_Render_Shapes* shapes = nullptr;
	glm::vec3 position;

	SChunk_Render_Buffer() = delete;
	SChunk_Render_Buffer(const SChunk_Render_Shapes* _shapes, const glm::vec3& _position)
	{
		shapes = _shapes;
		position = _position;
	}
};

/*Stock pour une texture tout les blocks qu'il va devoir rendre.*/
struct SChunk_Render_Data
{
	//Used to calculate global
	std::vector<SChunk_Render_Buffer*> renderBuffer;
	unsigned int verticesGlobalSize = 0;

	GLuint vertexsBuffer = 0;
	GLuint uvsBuffer = 0;

	//Used for the graphics generation
	std::vector<glm::vec3> globalVertexs;
	std::vector<glm::vec2> globalUVs;
};

class Chunk_Render
{
	friend class Chunk;
	friend class Chunks_Manager;
	friend class Chunk_Render_Generator;
	friend class Chunk_Data_Generator;
	friend class Chunk_Pool_Manager;

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
	ESide sideToUpdate;
	bool bHasFinishGeneration;
	bool bHasRender;
	
	/*Stock les rendu dans leur world position.*/
	SChunk_Render_Shapes**** allBlockShapes;

	/*Stock tout les datas à rendre. textures -> */
	std::map<GLuint, SChunk_Render_Data*> renderDatas;
};