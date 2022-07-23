#include "Chunk_Render.h"

#include "GlobalDefine.h"
#include "World.h"
#include "Chunk.h"

#include <glm/gtx/transform.hpp>
#include <common/controls.hpp>

Chunk_Render::Chunk_Render(Chunk* _currentChunk)
{
	ownerChunk = _currentChunk;
	ownerChunkPosition = ownerChunk->worldPosition;
}

Chunk_Render::~Chunk_Render()
{
	for each (const std::pair<GLuint, Chunk_Render_Data*>& _datas in renderDatas)
	{
		const Chunk_Render_Data* _renderBuffer = _datas.second;
		glDeleteBuffers(1, &_renderBuffer->vertexsBuffer);
		glDeleteBuffers(1, &_renderBuffer->uvsBuffer);
		delete _renderBuffer;
	}

	for (size_t x = 0; x < Chunk_Size; x++)
	{
		for (size_t y = 0; y < Chunk_Size; y++)
		{
			for (size_t z = 0; z < Chunk_Size; z++)
			{
				if (Chunk_Render_Shapes* _shapes = allBlockShapes[x][y][z])
					delete _shapes;
			}
		}
	}
}

void Chunk_Render::Render()
{
	for each (const std::pair<const GLuint&, const Chunk_Render_Data*>& _data in renderDatas)
	{
		const Chunk_Render_Data* _chunkRenderData(_data.second);

		glBindTexture(GL_TEXTURE_2D, _data.first);

		const glm::mat4& _MVP = getProjectionMatrix() * getViewMatrix() * glm::translate(glm::mat4(), ownerChunkPosition) * glm::mat4(1.0f);
		glUniformMatrix4fv(matrixID, 1, GL_FALSE, &_MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, _chunkRenderData->vertexsBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, _chunkRenderData->uvsBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, _chunkRenderData->verticesGlobalSize);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
	}
}

void Chunk_Render::RenderWithAlpha()
{

}