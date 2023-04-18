#include "Chunk_Render.h"

#include "GlobalDefine.h"
#include "World.h"
#include "Chunk.h"

#include <glm/gtx/transform.hpp>
#include <common/controls.hpp>

Chunk_Render::Chunk_Render(Chunk* _currentChunk)
{
	matrixID = 0;
	sideToUpdate = ESide::None;
	bHasFinishGeneration = false;
	bHasRender = false;
	allBlockShapes = nullptr;
	ownerChunk = _currentChunk;
	ownerChunkPosition = ownerChunk->worldPosition;
}

Chunk_Render::~Chunk_Render()
{
	for each (const std::pair<GLuint, SChunk_Render_Data*>& _datas in renderDatas)
	{
		SChunk_Render_Data* _renderBuffer = _datas.second;
		glDeleteBuffers(1, &_renderBuffer->vertexsBuffer);
		glDeleteBuffers(1, &_renderBuffer->uvsBuffer);

		std::vector<SChunk_Render_Buffer*>& _renderBufferPosition = _renderBuffer->renderBuffer;

		const size_t& _max = _renderBufferPosition.size();
		for (size_t i = 0; i < _max; ++i)
		{
			delete _renderBufferPosition[i];
		}

		delete _renderBuffer;
	}

	if (allBlockShapes)
	{
		for (size_t x = 0; x < Chunk_Size; ++x)
		{
			SChunk_Render_Shapes*** _x = allBlockShapes[x];
			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				SChunk_Render_Shapes** _y = _x[y];
				for (size_t z = 0; z < Chunk_Size; ++z)
				{
					if (SChunk_Render_Shapes* _shapes = _y[z])
						delete _shapes;
				}
				delete[] _y;
			}
			delete[] _x;
		}
		delete[] allBlockShapes;
	}
}

void Chunk_Render::Render()
{
	for each (const std::pair<const GLuint&, const SChunk_Render_Data*>& _data in renderDatas)
	{
		const SChunk_Render_Data* _chunkRenderData(_data.second);
		
		glBindTexture(GL_TEXTURE_2D, _data.first);

		const glm::mat4& _MVP = getProjectionMatrix() * getViewMatrix() * glm::translate(glm::mat4(), ownerChunkPosition) * glm::mat4(1.0f);
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &_MVP[0][0]);
		
		glBindBuffer(GL_ARRAY_BUFFER, _chunkRenderData->vertexsBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, _chunkRenderData->uvsBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, _chunkRenderData->verticesGlobalSize);
	}
}

void Chunk_Render::RenderWithAlpha()
{

}