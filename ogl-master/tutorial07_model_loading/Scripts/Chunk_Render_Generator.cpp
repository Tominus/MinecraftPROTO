#include "Chunk_Render_Generator.h"

#include "GlobalDefine.h"
#include "World.h"
#include "TextureLoader.h"
#include "Blocks_Global_Shapes.h"
#include "Chunks_Manager.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Block.h"

Chunk_Render_Generator::Chunk_Render_Generator(Chunks_Manager* _chunksManager)
{
	world = &World::Instance();
	matrixID = world->GetMatrixID();
	textureLoader = world->GetTextureLoader();
	blocksGlobalShapes = world->GetBlocksGlobalShapes();

	chunksManager = _chunksManager;
}

Chunk_Render_Generator::~Chunk_Render_Generator()
{
	
}

void Chunk_Render_Generator::GenerateNewChunkRender(Chunk_Render* _chunkRender, Chunk_Data* _chunkData)
{
	const glm::vec3& _chunkPosition = _chunkRender->ownerChunk->GetChunkPosition();
	Block****& _blocksData = _chunkData->blocks;

	_chunkRender->SetMatrixID(matrixID);

	//--- Will never change until other render type will be added.
	std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas = _chunkRender->renderDatas;


	SChunk_Render_Shapes****& _allBlockShapes = _chunkRender->allBlockShapes;
	_allBlockShapes = new SChunk_Render_Shapes***[Chunk_Size];

	glm::vec3 _blockPosition;
	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		SChunk_Render_Shapes***& _currentRenderShapesX = _allBlockShapes[x];
		_currentRenderShapesX = new SChunk_Render_Shapes**[Chunk_Size];

		_blockPosition.x = x;

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			SChunk_Render_Shapes**& _currentRenderShapesY = _currentRenderShapesX[y];
			_currentRenderShapesY = new SChunk_Render_Shapes*[Chunk_Size];

			_blockPosition.y = y;

			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				_blockPosition.z = z;
				Block*& _block = _blocksData[x][y][z];
				const EBlock_Type& _blockType = _block->GetBlockType();

				//--- Will be replaced when Global data finished
				const GLuint& _textureID = textureLoader->GetBlockTextureID(_blockType);
				const SBlock_Shape_Data* _shapesData = blocksGlobalShapes->GetBlockShapeData(EBlock_Shapes_Type::Block);
				
				const glm::vec3* _vertexs = _shapesData->GetVertexs();
				const glm::vec2* _uvs = _shapesData->GetUVs();

				SChunk_Render_Shapes* _currentShape = nullptr;
				if (x > 0 && y > 0 && z > 0 && x < Chunk_Max_Size && y < Chunk_Max_Size && z < Chunk_Max_Size)
				{
					
				}
				else
				{
					_currentShape = new SChunk_Render_Shapes(_vertexs, _uvs);
				}
				

				if (_currentShape)
				{
					SChunk_Render_Data* _currentChunkRenderData = GetChunkRenderData(_currentRenderDatas, _textureID);
					_currentChunkRenderData->verticesGlobalSize += _shapesData->GetVertexsSize();
					_currentChunkRenderData->renderBuffer.push_back(_currentShape);

					const size_t& _max = _shapesData->GetVertexsSize();
					for (size_t i = 0; i < _max; ++i)
					{
						_currentChunkRenderData->globalVertexs.push_back(_vertexs[i] + _blockPosition);
						_currentChunkRenderData->globalUVs.push_back(_uvs[i]);
					}
				}

				// TODO after ->
				//- Chaque face de mon block a une value allant de 1 à 32;
				//- j'additionne ces valeurs pour ensuite l'envoyer au Block_Global_Shapes(::Block, unsigned int);
				//- et il me retourne un Chunk_Render_Shapes*.
				//

				_currentRenderShapesY[z] = _currentShape;
			}
		}
	}
}

void Chunk_Render_Generator::GenerateChunkCGRender(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas)
{
	for each (const std::pair<const GLuint&, SChunk_Render_Data*>& _data in _currentRenderDatas)
	{
		SChunk_Render_Data* _render = _data.second;
		const unsigned& _verticesGlobalSize = _render->verticesGlobalSize;
		GLuint& _vertexsBuffer = _render->vertexsBuffer;
		GLuint& _uvsBuffer = _render->uvsBuffer;

		std::vector<glm::vec3>& _globalVertexs = _render->globalVertexs;
		std::vector<glm::vec2>& _globalUVs = _render->globalUVs;

		glGenBuffers(1, &_vertexsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexsBuffer);
		glBufferData(GL_ARRAY_BUFFER, _verticesGlobalSize * sizeof(glm::vec3), &_globalVertexs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &_uvsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _uvsBuffer);
		glBufferData(GL_ARRAY_BUFFER, _verticesGlobalSize * sizeof(glm::vec2), &_globalUVs[0], GL_STATIC_DRAW);

		_globalVertexs.clear();
		_globalUVs.clear();
	}
}

SChunk_Render_Data* Chunk_Render_Generator::GetChunkRenderData(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, const GLuint& _textureID)
{
	for each (const std::pair<const GLuint&, SChunk_Render_Data*>& _data in _currentRenderDatas)
	{
		if (_data.first == _textureID)
			return _data.second;
	}

	/*Texture id doesn't exist, so we create a new one.*/
	SChunk_Render_Data* _currentChunkRenderData = new SChunk_Render_Data();
	_currentRenderDatas.emplace(_textureID, _currentChunkRenderData);
	return _currentChunkRenderData;
}