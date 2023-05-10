#include "Chunk_Render_Generator.h"

#include "GlobalDefine.h"
#include "World.h"
#include "TextureLoader.h"
#include "Blocks_Global_Shapes.h"
#include "Blocks_Global_Datas.h"
#include "Chunks_Manager.h"
#include "Chunk_Pool_Manager.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_SideData.h"
#include "Block.h"
#include "Block_Datas.h"
#include "Shaders_Manager.h"

#define Check_Block(_block) blocksGlobalDatas->GetBlockData(_block->GetBlockType())->blockRenderType != EBlock_Render_Type::Opaque
#define Debug_Out_Of_Bound(_something) _something.x > Chunk_Size || _something.y > Chunk_Size || _something.z > Chunk_Size || _something.x < 0 || _something.y < 0 || _something.z < 0

Chunk_Render_Generator::Chunk_Render_Generator(Chunks_Manager* _chunksManager)
{
	world = World::Instance();
	matrixID = world->GetShadersManager()->GetMatrixID();
	textureLoader = world->GetTextureLoader();
	blockGlobalShapes = world->GetBlocksGlobalShapes();
	blocksGlobalDatas = world->GetBlocksGlobalDatas();

	chunksManager = _chunksManager;
	chunkPoolManager = nullptr;

	fMinHeightChunk = Chunk_Min_World_Height + 2;
	fMaxHeightChunk = Chunk_Max_World_Height - 2;

	mutex_ChunkManager = chunksManager->mutex;
}

Chunk_Render_Generator::~Chunk_Render_Generator()
{

}

Threaded void Chunk_Render_Generator::GenerateNewChunkRender(Chunk_Render* _chunkRender, Chunk_Data* _chunkData)
{
	WaitForSingleObject(mutex_ChunkManager, INFINITE);
	Chunk* _ownerChunk = _chunkRender->ownerChunk;
	const glm::vec3& _chunkPosition = _ownerChunk->GetChunkPosition();
	Chunk_SideData* _sideData = _ownerChunk->chunkSideData;
	ReleaseMutex(mutex_ChunkManager);

	const float& _chunkHeight = _chunkPosition.y;
	Block*** _downSide = _sideData->downBlocks;
	Block*** _upSide = _sideData->upBlocks;
	Block*** _leftSide = _sideData->leftBlocks;
	Block*** _rightSide = _sideData->rightBlocks;
	Block*** _backSide = _sideData->backBlocks;
	Block*** _frontSide = _sideData->frontBlocks;

	Block****& _blocksData = _chunkData->blocks;

	_chunkRender->SetMatrixID(matrixID);

	//--- Will never change until other render type will be added.
	std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas = _chunkRender->renderDatas;


	SChunk_Render_Shapes****& _allBlockShapes = _chunkRender->allBlockShapes;
	glm::vec3 _blockPosition;

	for (int x = 0; x < Chunk_Size; ++x)
	{
		SChunk_Render_Shapes***& _currentRenderShapesX = _allBlockShapes[x];
		_blockPosition.x = x;

		for (int y = 0; y < Chunk_Size; ++y)
		{
			SChunk_Render_Shapes**& _currentRenderShapesY = _currentRenderShapesX[y];
			_blockPosition.y = y;

			for (int z = 0; z < Chunk_Size; ++z)
			{
				SChunk_Render_Shapes*& _currentShape = _currentRenderShapesY[z];
				_blockPosition.z = z;

				Block* _block = _blocksData[x][y][z];
				const EBlock_Type& _blockType = _block->GetBlockType();

				const SBlock_Datas const* _blockData = blocksGlobalDatas->GetBlockData(_blockType);
				const GLuint& _textureID = textureLoader->GetBlockTextureID(_blockType);

				unsigned _index = 0;

				if (_blockData->blockRenderType == EBlock_Render_Type::No_Render)
				{
					//No Render
					if (_currentShape)
					{
						chunkPoolManager->RetreiveChunkRenderShapes(_currentShape);
						_currentShape = nullptr;
					}
				}
				else
				{
					if (x > 0 && y > 0 && z > 0 && x < Chunk_Max_Size && y < Chunk_Max_Size && z < Chunk_Max_Size)
					{
						//in chunk

						//Down
						if (Check_Block(_blocksData[x][y - 1][z]))
						{
							_index += 1;
						}

						//Up
						if (Check_Block(_blocksData[x][y + 1][z]))
						{
							_index += 2;
						}

						//Left
						if (Check_Block(_blocksData[x - 1][y][z]))
						{
							_index += 4;
						}

						//Right
						if (Check_Block(_blocksData[x + 1][y][z]))
						{
							_index += 8;
						}

						//Back
						if (Check_Block(_blocksData[x][y][z - 1]))
						{
							_index += 16;
						}

						//Front
						if (Check_Block(_blocksData[x][y][z + 1]))
						{
							_index += 32;
						}
					}
					else
					{
						//chunk border

						//Down
						if (y - 1 > -1)
						{
							if (Check_Block(_blocksData[x][y - 1][z]))
							{
								_index += 1;
							}
						}
						else
						{
							if (_downSide)
							{
								if (Check_Block(_downSide[x][z]))
								{
									_index += 1;
								}
							}
							else
							{
								//Check if chunk is at the bottom
								if (_chunkHeight < fMinHeightChunk)
									_index += 1;
							}
						}

						//Up
						if (y + 1 < Chunk_Size)
						{
							if (Check_Block(_blocksData[x][y + 1][z]))
							{
								_index += 2;
							}
						}
						else
						{
							if (_upSide)
							{
								if (Check_Block(_upSide[x][z]))
								{
									_index += 2;
								}
							}
							else
							{
								//Check if chunk is at the top
								if (_chunkHeight > fMaxHeightChunk)
									_index += 2;
							}
						}

						//Left
						if (x - 1 > -1)
						{
							if (Check_Block(_blocksData[x - 1][y][z]))
							{
								_index += 4;
							}
						}
						else
						{
							if (Check_Block(_leftSide[y][z]))
							{
								_index += 4;
							}
						}

						//Right
						if (x + 1 < Chunk_Size)
						{
							if (Check_Block(_blocksData[x + 1][y][z]))
							{
								_index += 8;
							}
						}
						else
						{
							if (Check_Block(_rightSide[y][z]))
							{
								_index += 8;
							}
						}

						//Back
						if (z - 1 > -1)
						{
							if (Check_Block(_blocksData[x][y][z - 1]))
							{
								_index += 16;
							}
						}
						else
						{
							if (Check_Block(_backSide[x][y]))
							{
								_index += 16;
							}
						}

						//Front
						if (z + 1 < Chunk_Size)
						{
							if (Check_Block(_blocksData[x][y][z + 1]))
							{
								_index += 32;
							}
						}
						else
						{
							if (Check_Block(_frontSide[x][y]))
							{
								_index += 32;
							}
						}
					}

					if (const SBlock_Shape_Data* _shapesData = blockGlobalShapes->GetBlockVertexsAndUVs(_blockData->blockShapeType, _index))
					{
						const glm::vec3* _vertexs = _shapesData->GetVertexs();
						const glm::vec2* _uvs = _shapesData->GetUVs();
						const size_t* _vertexsSize = _shapesData->GetVertexsSize();

						while (_currentShape == false)
						{
							_currentShape = chunkPoolManager->GetChunkRenderShapes();

							if (_currentShape)break;

							chunkPoolManager->SetHasToGenerateNewChunkRenderShapes();
							Sleep(16);
						}

						_currentShape->SetRenderShapes(_vertexs, _uvs, _vertexsSize);


						SChunk_Render_Data* _currentChunkRenderData = GetChunkRenderData(_currentRenderDatas, _textureID);
						_currentChunkRenderData->verticesGlobalSize += *_vertexsSize;

						SChunk_Render_Buffer* _currentRenderBuffer = chunkPoolManager->GetChunkRenderBuffer();

						while (_currentRenderBuffer == false)
						{
							chunkPoolManager->SetHasToGenerateNewChunkRenderBuffer();
							Sleep(16);

							_currentRenderBuffer = chunkPoolManager->GetChunkRenderBuffer();
						}

						_currentRenderBuffer->SetRenderBuffer(_currentShape, _blockPosition);

						_currentChunkRenderData->renderBuffer.push_back(_currentRenderBuffer);

						const size_t& _max = *_vertexsSize;
						for (size_t i = 0; i < _max; ++i)
						{
							_currentChunkRenderData->globalVertexs.push_back(_vertexs[i] + _blockPosition);
							_currentChunkRenderData->globalUVs.push_back(_uvs[i]);
						}
					}
				}
			}
		}
	}

	//Check if has render
	_chunkRender->bHasRender = _currentRenderDatas.size() > 0;
}

SChunk_Render_Data* Chunk_Render_Generator::GetChunkRenderData(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, const GLuint& _textureID)
{
	for each (const std::pair<const GLuint&, SChunk_Render_Data*>&_data in _currentRenderDatas)
	{
		if (_data.first == _textureID)
			return _data.second;
	}

	SChunk_Render_Data* _currentChunkRenderData = chunkPoolManager->GetChunkRenderData();
	while (_currentChunkRenderData == false)
	{
		chunkPoolManager->SetHasToGenerateNewChunkRenderData();
		Sleep(16);

		_currentChunkRenderData = chunkPoolManager->GetChunkRenderData();
	}

	/*Texture id doesn't exist, so we create a new one.*/
	_currentRenderDatas.emplace(_textureID, _currentChunkRenderData);
	return _currentChunkRenderData;
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

void Chunk_Render_Generator::AddTextureToUpdate(std::vector<GLuint>& _texturesToUpdate, const GLuint& _textureID)
{
	const size_t& _max = _texturesToUpdate.size();
	for (size_t i = 0; i < _max; ++i)
	{
		if (_texturesToUpdate[i] == _textureID)
			return;
	}

	_texturesToUpdate.push_back(_textureID);
}

void Chunk_Render_Generator::RegenerateRender(Chunk* _chunk, std::vector<GLuint>& _texturesToUpdate)
{
	Chunk_Render* _chunkRender = _chunk->chunkRender;

	std::map<GLuint, SChunk_Render_Data*>& _renderDatas = _chunkRender->renderDatas;

	const size_t& _texturesSize = _texturesToUpdate.size();
	for (size_t i = 0; i < _texturesSize; ++i)
	{
		SChunk_Render_Data* _chunkRenderData = GetChunkRenderData(_renderDatas, _texturesToUpdate[i]);

		glDeleteBuffers(1, &_chunkRenderData->vertexsBuffer);
		glDeleteBuffers(1, &_chunkRenderData->uvsBuffer);
		unsigned& _verticesGlobalSize = _chunkRenderData->verticesGlobalSize;
		_verticesGlobalSize = 0;

		std::vector<SChunk_Render_Buffer*>& _renderBuffers = _chunkRenderData->renderBuffer;

		const size_t& _bufferSize = _renderBuffers.size();
		for (size_t j = 0; j < _bufferSize; ++j)
		{
			SChunk_Render_Buffer* _renderBuffer = _renderBuffers[j];
			const SChunk_Render_Shapes* _shapes = _renderBuffer->shapes;
			const glm::vec3& _position = _renderBuffer->position;

			const glm::vec3* _vertexs = _shapes->vertexs;
			const glm::vec2* _uvs = _shapes->uvs;
			const size_t& _vertexsSize = *_shapes->vertexsSize;

			_verticesGlobalSize += _vertexsSize;

			for (size_t k = 0; k < _vertexsSize; ++k)
			{
				_chunkRenderData->globalVertexs.push_back(_vertexs[k] + _position);
				_chunkRenderData->globalUVs.push_back(_uvs[k]);
			}
		}
	}

	GenerateChunkUpdatedCGRender(_renderDatas, _texturesToUpdate);
}

Threaded void Chunk_Render_Generator::GenerateChunkUpdatedCGRender(std::map<GLuint, SChunk_Render_Data*>& _currentRenderDatas, std::vector<GLuint>& _texturesToUpdate)
{
	const size_t& _max = _texturesToUpdate.size();
	for (size_t i = 0; i < _max; ++i)
	{
		SChunk_Render_Data* _renderData = _currentRenderDatas.at(_texturesToUpdate[i]);
		const unsigned& _verticesGlobalSize = _renderData->verticesGlobalSize;

		if (_verticesGlobalSize > 0)
		{
			GLuint& _vertexsBuffer = _renderData->vertexsBuffer;
			GLuint& _uvsBuffer = _renderData->uvsBuffer;

			std::vector<glm::vec3>& _globalVertexs = _renderData->globalVertexs;
			std::vector<glm::vec2>& _globalUVs = _renderData->globalUVs;

			glGenBuffers(1, &_vertexsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, _vertexsBuffer);
			glBufferData(GL_ARRAY_BUFFER, _verticesGlobalSize * sizeof(glm::vec3), &_globalVertexs[0], GL_STATIC_DRAW);

			glGenBuffers(1, &_uvsBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, _uvsBuffer);
			glBufferData(GL_ARRAY_BUFFER, _verticesGlobalSize * sizeof(glm::vec2), &_globalUVs[0], GL_STATIC_DRAW);

			_globalVertexs.clear();
			_globalUVs.clear();
		}
		else
		{
			_currentRenderDatas.erase(_texturesToUpdate[i]);
			delete _renderData;

			printf("[Chunk_Render_Generator::GenerateChunkUpdatedCGRender] -> Last render block side is deleted");
			continue;
		}
	}
}

void Chunk_Render_Generator::ClearUnUsedChunkCGRender()
{
	//TODO if last block of a render is destroy
}