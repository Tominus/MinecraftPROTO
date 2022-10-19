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
	const std::vector<std::vector<std::vector<Block*>>>& _blocksData = _chunkData->blocks;
	
	_chunkRender->SetMatrixID(matrixID);

	/*--- Will never change until other render type will be added.*/
	const Block_Shape_Data* _shapesData = blocksGlobalShapes->GetBlockShapeData(Block_Shapes_Type::Block);


	std::vector<std::vector<std::vector<Chunk_Render_Shapes*>>>& _allBlockShapes = _chunkRender->allBlockShapes;
	std::map<GLuint, Chunk_Render_Data*>& _currentRenderDatas = _chunkRender->renderDatas;

	glm::vec3 _blockPosition;
	for (size_t x = 0; x < Chunk_Size; ++x)
	{
		std::vector<std::vector<Chunk_Render_Shapes*>> _currentRenderShapesYZ;
		_blockPosition.x = x;

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			std::vector<Chunk_Render_Shapes*> _currentRenderShapesZ;
			_blockPosition.y = y;

			for (size_t z = 0; z < Chunk_Size; ++z)
			{
				_blockPosition.z = z;
				const Block_Type& _blockType = _blocksData[x][y][z]->GetBlockType();

				//--- Will be replaced when Global data finished
				const GLuint& _textureID = textureLoader->GetBlockTextureID(_blockType);
				

				//render du blockShapes pour x y z si on doit faire un rendu sinon on met nullptr
				const std::vector<glm::vec3>* _vertexs = _shapesData->GetVertexs();
				const std::vector<glm::vec2>* _uvs = _shapesData->GetUVs();
				Chunk_Render_Shapes* _currentShape = new Chunk_Render_Shapes(_vertexs, _uvs);
				
				Chunk_Render_Data* _currentChunkRenderData = GetChunkRenderData(_currentRenderDatas, _textureID);
				_currentChunkRenderData->verticesGlobalSize += _shapesData->GetVertexsSize();
				_currentChunkRenderData->renderBuffer.push_back(_currentShape);

				if (_currentShape)
				{
					const size_t& _max = _vertexs->size();
					for (size_t i = 0; i < _max; ++i)
					{
						_currentChunkRenderData->globalVertexs.push_back(_vertexs->at(i) + _blockPosition);
						_currentChunkRenderData->globalUVs.push_back(_uvs->at(i));
					}
				}

				/* TODO after ->
				- Chaque face de mon block a une value allant de 1 à 32;
				- j'additionne ces valeurs pour ensuite l'envoyer au Block_Global_Shapes(::Block, unsigned int);
				- et il me retourne un Chunk_Render_Shapes*.
				*/
				
				_currentRenderShapesZ.push_back(_currentShape);
			}

			_currentRenderShapesYZ.push_back(_currentRenderShapesZ);
		}

		_allBlockShapes.push_back(_currentRenderShapesYZ);
	}	
}

void Chunk_Render_Generator::GenerateChunkCGRender(std::map<GLuint, Chunk_Render_Data*>& _currentRenderDatas)
{
	for each (const std::pair<const GLuint&, Chunk_Render_Data*>&_data in _currentRenderDatas)
	{
		Chunk_Render_Data* _render = _data.second;

		glGenBuffers(1, &_render->vertexsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _render->vertexsBuffer);
		glBufferData(GL_ARRAY_BUFFER, _render->verticesGlobalSize * sizeof(glm::vec3), &_render->globalVertexs[0], GL_STATIC_DRAW);

		glGenBuffers(1, &_render->uvsBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _render->uvsBuffer);
		glBufferData(GL_ARRAY_BUFFER, _render->verticesGlobalSize * sizeof(glm::vec2), &_render->globalUVs[0], GL_STATIC_DRAW);

		_render->globalVertexs.clear();
		_render->globalUVs.clear();
	}
}

Chunk_Render_Data* Chunk_Render_Generator::GetChunkRenderData(std::map<GLuint, Chunk_Render_Data*>& _currentRenderDatas, const GLuint& _textureID)
{
	for each (const std::pair<const GLuint&, Chunk_Render_Data*>& _data in _currentRenderDatas)
	{
		if (_data.first == _textureID)
			return _data.second;
	}

	/*Texture id doesn't exist, so we create a new one.*/
	Chunk_Render_Data* _currentChunkRenderData = new Chunk_Render_Data();
	_currentRenderDatas.emplace(_textureID, _currentChunkRenderData);
	return _currentChunkRenderData;
}