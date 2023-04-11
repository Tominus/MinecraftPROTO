#include "Debug_World.h"
#include "GlobalDefine.h"
#include "World.h"
#include "Shaders_Manager.h"

#include <common/controls.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


Debug_World::Debug_World(GLFWwindow* _window)
{
	window = _window;

	Shaders_Manager* _shadersManager = &Shaders_Manager::Instance();
	matrixID_Debug = _shadersManager->GetMatrixID_Debug();
	matrixID_Color_Debug = _shadersManager->GetMatrixID_Color_Debug();
	programID_Debug = _shadersManager->GetProgramID_Debug();

	bDebugDrawChunk = false;
	vertexsBufferDrawChunk = 0;
	vertexsSizeDrawChunk = 0;

	fChunkSize = (float)Chunk_Size;
	fHalfChunkSize = fChunkSize / 2.f;
	fDoubleChunkSize = fChunkSize * 2.f;

	debugColor = new float[4]{ 0.f, 1.f, 0.f, 1.f };
}

Debug_World::~Debug_World()
{
	delete[] debugColor;
	glDeleteBuffers(1, &vertexsBufferDrawChunk);
}

void Debug_World::InitDebug()
{
	std::vector<glm::vec3> _vertices{
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(fChunkSize, 0.f, 0.f),

		glm::vec3(fChunkSize, 0.f, 0.f),
		glm::vec3(fChunkSize, 0.f, fChunkSize),

		glm::vec3(fChunkSize, 0.f, fChunkSize),
		glm::vec3(0.f, 0.f, fChunkSize),

		glm::vec3(0.f, 0.f, fChunkSize),
		glm::vec3(0.f, 0.f, 0.f),

		//-> y
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, fChunkSize, 0.f),

		glm::vec3(fChunkSize, 0.f, 0.f),
		glm::vec3(fChunkSize, fChunkSize, 0.f),

		glm::vec3(fChunkSize, 0.f, fChunkSize),
		glm::vec3(fChunkSize, fChunkSize, fChunkSize),

		glm::vec3(0.f, 0.f, fChunkSize),
		glm::vec3(0.f, fChunkSize, fChunkSize),

		//y
		glm::vec3(0.f, fChunkSize, 0.f),
		glm::vec3(fChunkSize, fChunkSize, 0.f),

		glm::vec3(fChunkSize, fChunkSize, 0.f),
		glm::vec3(fChunkSize, fChunkSize, fChunkSize),

		glm::vec3(fChunkSize, fChunkSize, fChunkSize),
		glm::vec3(0.f, fChunkSize, fChunkSize),

		glm::vec3(0.f, fChunkSize, fChunkSize),
		glm::vec3(0.f, fChunkSize, 0.f),
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

	glUseProgram(programID_Debug);

	float _color[4]{ 0.f, 1.f, 1.f, 1.f};

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
				glUniformMatrix4fv(matrixID_Debug, 1, GL_FALSE, &_MVP[0][0]);
				glUniformMatrix4fv(matrixID_Color_Debug, 1, GL_FALSE, _color);

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

	const glm::vec3& _playerPosition = getPosition() - glm::vec3(fHalfChunkSize);
	const float& _xPos = round(_playerPosition.x / fChunkSize);
	       float _yPos = round(_playerPosition.y / fChunkSize);
	const float& _zPos = round(_playerPosition.z / fChunkSize);

	if (_yPos < Chunk_Zero_World_Height + 1)
		_yPos = Chunk_Zero_World_Height + 1;
	else if (_yPos > Chunk_Max_World_Height - 2)
		_yPos = Chunk_Max_World_Height - 2;

	const glm::vec3 _playerPositionChunkRelative(_xPos * fChunkSize, _yPos * fChunkSize, _zPos * fChunkSize);

	glUseProgram(programID_Debug);

	glm::vec3 _offset;
	glEnableVertexAttribArray(0);
	for (float x = -fChunkSize; x < fDoubleChunkSize; x += fChunkSize)
	{
		_offset.x = x;
		for (float y = -fChunkSize; y < fDoubleChunkSize; y += fChunkSize)
		{
			_offset.y = y;
			for (float z = -fChunkSize; z < fDoubleChunkSize; z += fChunkSize)
			{
				_offset.z = z;
				const glm::mat4& _MVP = getProjectionMatrix() * getViewMatrix() * glm::translate(glm::mat4(), _playerPositionChunkRelative + _offset) * glm::mat4(1.f);
				glUniformMatrix4fv(matrixID_Debug, 1, GL_FALSE, &_MVP[0][0]);
				glUniform4fv(matrixID_Color_Debug, 1, debugColor);

				glBindBuffer(GL_ARRAY_BUFFER, vertexsBufferDrawChunk);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

				glDrawArrays(GL_LINES, 0, vertexsSizeDrawChunk);
			}
		}
	}
	glDisableVertexAttribArray(0);
}