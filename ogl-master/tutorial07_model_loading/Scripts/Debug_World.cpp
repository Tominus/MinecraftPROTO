#include "Debug_World.h"
#include <common/controls.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "GlobalDefine.h"


Debug_World::Debug_World(GLFWwindow* _window)
{
	window = _window;
	matrixID = 0;
	bDebugDrawChunk = false;
	vertexsBufferDrawChunk = 0;
	vertexsSizeDrawChunk = 0;
}

Debug_World::~Debug_World()
{
	glDeleteBuffers(1, &vertexsBufferDrawChunk);
}

void Debug_World::InitDebug(const GLuint& _matrixID)
{
	matrixID = _matrixID;

	const float& _chunkSize = (float)Chunk_Size;

	std::vector<glm::vec3> _vertices{
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(_chunkSize, 0.f, 0.f),

		glm::vec3(_chunkSize, 0.f, 0.f),
		glm::vec3(_chunkSize, 0.f, _chunkSize),

		glm::vec3(_chunkSize, 0.f, _chunkSize),
		glm::vec3(0.f, 0.f, _chunkSize),

		glm::vec3(0.f, 0.f, _chunkSize),
		glm::vec3(0.f, 0.f, 0.f),

		//-> y
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, _chunkSize, 0.f),

		glm::vec3(_chunkSize, 0.f, 0.f),
		glm::vec3(_chunkSize, _chunkSize, 0.f),

		glm::vec3(_chunkSize, 0.f, _chunkSize),
		glm::vec3(_chunkSize, _chunkSize, _chunkSize),

		glm::vec3(0.f, 0.f, _chunkSize),
		glm::vec3(0.f, _chunkSize, _chunkSize),

		//y
		glm::vec3(0.f, _chunkSize, 0.f),
		glm::vec3(_chunkSize, _chunkSize, 0.f),

		glm::vec3(_chunkSize, _chunkSize, 0.f),
		glm::vec3(_chunkSize, _chunkSize, _chunkSize),

		glm::vec3(_chunkSize, _chunkSize, _chunkSize),
		glm::vec3(0.f, _chunkSize, _chunkSize),

		glm::vec3(0.f, _chunkSize, _chunkSize),
		glm::vec3(0.f, _chunkSize, 0.f),
	};

	vertexsSizeDrawChunk = _vertices.size();

	glGenBuffers(1, &vertexsBufferDrawChunk);
	glBindBuffer(GL_ARRAY_BUFFER, vertexsBufferDrawChunk);
	glBufferData(GL_ARRAY_BUFFER, vertexsSizeDrawChunk * sizeof(glm::vec3), &_vertices[0], GL_STATIC_DRAW);
}

void Debug_World::UpdateDebug()
{
	Debug_DrawDebugScaledChunk();
}

void Debug_World::Debug_DrawChunk()
{
	if (glfwGetKey(window, GLFW_KEY_G) && glfwGetKey(window, GLFW_KEY_F3))
		bDebugDrawChunk = !bDebugDrawChunk;

	if (!bDebugDrawChunk)return;

	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8, 8, 8);
	const float& _xPos = round(_playerPosition.x / 16.f);
	const float& _yPos = round(_playerPosition.y / 16.f);
	const float& _zPos = round(_playerPosition.z / 16.f);
	
	const glm::vec3 _playerPositionChunkRelative(_xPos * 16.f, _yPos * 16.f, _zPos * 16.f);


	glm::vec3 _offset;
	glEnableVertexAttribArray(0);
	for (float x = -16.f; x < 32.f; x += 16.f)
	{
		_offset.x = x;
		for (float y = -16.f; y < 32.f; y += 16.f)
		{
			_offset.y = y;
			for (float z = -16.f; z < 32.f; z += 16.f)
			{
				_offset.z = z;
				const glm::mat4& _MVP = getProjectionMatrix() * getViewMatrix() * glm::translate(glm::mat4(), _playerPositionChunkRelative + _offset) * glm::mat4(1.0f);
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &_MVP[0][0]);

				glBindBuffer(GL_ARRAY_BUFFER, vertexsBufferDrawChunk);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glDrawArrays(GL_LINES, 0, vertexsSizeDrawChunk);
			}
		}
	}
	glDisableVertexAttribArray(0);
}

void Debug_World::Debug_DrawDebugScaledChunk()
{
	if (glfwGetKey(window, GLFW_KEY_G) && glfwGetKey(window, GLFW_KEY_F3))
		bDebugDrawChunk = !bDebugDrawChunk;

	if (!bDebugDrawChunk)return;

	const float& _chunkSize = (float)Chunk_Size;

	const glm::vec3& _playerPosition = getPosition() - glm::vec3(_chunkSize / 2.f, _chunkSize / 2.f, _chunkSize / 2.f);
	const float& _xPos = round(_playerPosition.x / _chunkSize);
	const float& _yPos = round(_playerPosition.y / _chunkSize);
	const float& _zPos = round(_playerPosition.z / _chunkSize);

	const glm::vec3 _playerPositionChunkRelative(_xPos * _chunkSize, _yPos * _chunkSize, _zPos * _chunkSize);


	glm::vec3 _offset;
	glEnableVertexAttribArray(0);
	for (float x = -_chunkSize; x < _chunkSize * 2.f; x += _chunkSize)
	{
		_offset.x = x;
		for (float y = -_chunkSize; y < _chunkSize * 2.f; y += _chunkSize)
		{
			_offset.y = y;
			for (float z = -_chunkSize; z < _chunkSize * 2.f; z += _chunkSize)
			{
				_offset.z = z;
				const glm::mat4& _MVP = getProjectionMatrix() * getViewMatrix() * glm::translate(glm::mat4(), _playerPositionChunkRelative + _offset) * glm::mat4(1.0f);
				glUniformMatrix4fv(matrixID, 1, GL_FALSE, &_MVP[0][0]);

				glBindBuffer(GL_ARRAY_BUFFER, vertexsBufferDrawChunk);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glDrawArrays(GL_LINES, 0, vertexsSizeDrawChunk);
			}
		}
	}
	glDisableVertexAttribArray(0);
}