#include "Chunk_Render_Generator.h"

#include "GlobalDefine.h"
#include "World.h"
#include "TextureLoader.h"
#include "Blocks_Global_Shapes.h"
#include "Blocks_Global_Datas.h"
#include "Chunks_Manager.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Block.h"
#include "Block_Datas.h"

#define Check_Block(_block) blocksGlobalDatas->GetBlockData(_block->GetBlockType())->blockRenderType != EBlock_Render_Type::Opaque

Chunk_Render_Generator::Chunk_Render_Generator(Chunks_Manager* _chunksManager)
{
	world = &World::Instance();
	matrixID = world->GetMatrixID();
	textureLoader = world->GetTextureLoader();
	blockGlobalShapes = world->GetBlocksGlobalShapes();
	blocksGlobalDatas = world->GetBlocksGlobalDatas();

	chunksManager = _chunksManager;

	mutex_All_ChunkManager = chunksManager->allMutex;
	mutex_ChunkManager = chunksManager->mutex;
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
	for (int x = 0; x < Chunk_Size; ++x)
	{
		SChunk_Render_Shapes***& _currentRenderShapesX = _allBlockShapes[x];
		_currentRenderShapesX = new SChunk_Render_Shapes**[Chunk_Size];

		_blockPosition.x = x;

		for (int y = 0; y < Chunk_Size; ++y)
		{
			SChunk_Render_Shapes**& _currentRenderShapesY = _currentRenderShapesX[y];
			_currentRenderShapesY = new SChunk_Render_Shapes*[Chunk_Size];

			_blockPosition.y = y;

			for (int z = 0; z < Chunk_Size; ++z)
			{
				_blockPosition.z = z;
				Block*& _block = _blocksData[x][y][z];
				const EBlock_Type& _blockType = _block->GetBlockType();

				const SBlock_Datas const* _blockData = blocksGlobalDatas->GetBlockData(_blockType);

				//--- Will be replaced when Global data finished
				const GLuint& _textureID = textureLoader->GetBlockTextureID(_blockType);
				const SBlock_Shape_Data* _shapesData = nullptr;
				
				const glm::vec3* _vertexs = nullptr;
				const glm::vec2* _uvs = nullptr;

				unsigned _index = 0;

				SChunk_Render_Shapes* _currentShape = nullptr;
				if (_blockData->blockRenderType == EBlock_Render_Type::No_Render)
				{
					//No Render
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _downChunk = _chunkData->downChunk)
							{
								if (Check_Block(_downChunk->GetChunkData()->GetBlock(glm::uvec3(x, Chunk_Max_Size, z))))
								{
									_index += 1;
								}
							}
							else
							{
								_index += 1;
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _upChunk = _chunkData->upChunk)
							{
								if (Check_Block(_upChunk->GetChunkData()->GetBlock(glm::uvec3(x, 0, z))))
								{
									_index += 2;
								}
							}
							else
							{
								_index += 2;
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _leftChunk = _chunkData->leftChunk)
							{
								if (Check_Block(_leftChunk->GetChunkData()->GetBlock(glm::uvec3(Chunk_Max_Size, y, z))))
								{
									_index += 4;
								}
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _rightChunk = _chunkData->rightChunk)
							{
								if (Check_Block(_rightChunk->GetChunkData()->GetBlock(glm::uvec3(0, y, z))))
								{
									_index += 8;
								}
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _backChunk = _chunkData->backChunk)
							{
								if (Check_Block(_backChunk->GetChunkData()->GetBlock(glm::uvec3(x, y, Chunk_Max_Size))))
								{
									_index += 16;
								}
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
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
							WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);
							if (Chunk* _frontChunk = _chunkData->frontChunk)
							{
								if (Check_Block(_frontChunk->GetChunkData()->GetBlock(glm::uvec3(x, y, 0))))
								{
									_index += 32;
								}
							}

							ReleaseMutex(mutex_All_ChunkManager[0]);
							ReleaseMutex(mutex_All_ChunkManager[1]);
						}
					}

					if (_shapesData = blockGlobalShapes->GetBlockVertexsAndUVs(_blockData->blockShapeType, _index))
					{
						_vertexs = _shapesData->GetVertexs();
						_uvs = _shapesData->GetUVs();
						const size_t& _vertexsSize = _shapesData->GetVertexsSize();
						_currentShape = new SChunk_Render_Shapes(_vertexs, _uvs);


						SChunk_Render_Data* _currentChunkRenderData = GetChunkRenderData(_currentRenderDatas, _textureID);

						_currentChunkRenderData->verticesGlobalSize += _vertexsSize;
						_currentChunkRenderData->renderBuffer.push_back(_currentShape);

						for (size_t i = 0; i < _vertexsSize; ++i)
						{
							_currentChunkRenderData->globalVertexs.push_back(_vertexs[i] + _blockPosition);
							_currentChunkRenderData->globalUVs.push_back(_uvs[i]);
						}
					}
				}

				_currentRenderShapesY[z] = _currentShape;
			}
		}
	}

	SetAllSideChunkForUpdate(_chunkData);
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

void Chunk_Render_Generator::SetAllSideChunkForUpdate(Chunk_Data* _chunkData)
{
	WaitForMultipleObjects(2, mutex_All_ChunkManager, TRUE, INFINITE);

	Chunk* _downChunk = _chunkData->downChunk;
	Chunk* _upChunk = _chunkData->upChunk;
	Chunk* _leftChunk = _chunkData->leftChunk;
	Chunk* _rightChunk = _chunkData->rightChunk;
	Chunk* _backChunk = _chunkData->backChunk;
	Chunk* _frontChunk = _chunkData->frontChunk;

	if (_downChunk)
	{
		_downChunk->chunkRender->sideToUpdate |= ESide::Down;
		chunksManager->AddWaitingForSideUpdateChunk(_downChunk);
	}
	if (_upChunk)
	{
		_upChunk->chunkRender->sideToUpdate |= ESide::Up;
		chunksManager->AddWaitingForSideUpdateChunk(_upChunk);
	}

	if (_leftChunk)
	{
		_leftChunk->chunkRender->sideToUpdate |= ESide::Left;
		chunksManager->AddWaitingForSideUpdateChunk(_leftChunk);
	}
	if (_rightChunk)
	{
		_rightChunk->chunkRender->sideToUpdate |= ESide::Right;
		chunksManager->AddWaitingForSideUpdateChunk(_rightChunk);
	}

	if (_backChunk)
	{
		_backChunk->chunkRender->sideToUpdate |= ESide::Back;
		chunksManager->AddWaitingForSideUpdateChunk(_backChunk);
	}
	if (_frontChunk)
	{
		_frontChunk->chunkRender->sideToUpdate |= ESide::Front;
		chunksManager->AddWaitingForSideUpdateChunk(_frontChunk);
	}

	ReleaseMutex(mutex_All_ChunkManager[0]);
	ReleaseMutex(mutex_All_ChunkManager[1]);
}

void Chunk_Render_Generator::UpdateChunkSideRender(Chunk* _chunk)
{
	//Chunk_Data*& _chunkData = _chunk->chunkData;
	//Chunk_Render*& _chunkRender = _chunk->chunkRender;



}