#include "Shaders_Manager.h"

#include <common/shader.hpp>

Shaders_Manager::Shaders_Manager()
{
	programID = 0;
	matrixID = 0;
	vertexArrayID = 0;

	programID_Debug = 0;
	matrixID_Debug = 0;
	matrixID_Color_Debug = 0;
}

Shaders_Manager::~Shaders_Manager()
{
	glDeleteProgram(programID);
	glDeleteProgram(programID_Debug);
	glDeleteVertexArrays(1, &vertexArrayID);
}

void Shaders_Manager::Initialize()
{
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
	matrixID = glGetUniformLocation(programID, "MVP");

	programID_Debug = LoadShaders("Shader_Vertex_Debug.vertexshader", "Shader_Fragment_Debug.fragmentshader");
	matrixID_Debug = glGetUniformLocation(programID_Debug, "MVP_Debug");
	matrixID_Color_Debug = glGetUniformLocation(programID_Debug, "MVP_Color_Debug");
}