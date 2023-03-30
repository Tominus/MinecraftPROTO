#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glfw3.h>

class Shaders_Manager
{
	
	friend class World;

private:
	Shaders_Manager();
	~Shaders_Manager();

	void Initialize();

public:
	inline static Shaders_Manager& Instance()
	{
		static Shaders_Manager _shaderManager;
		return _shaderManager;
	}

	inline const GLuint& GetMatrixID() const { return matrixID; }
	inline const GLuint& GetProgramID() const { return programID; }

	inline const GLuint& GetMatrixID_Debug() const { return matrixID_Debug; }
	inline const GLuint& GetMatrixID_Color_Debug() const { return matrixID_Color_Debug; }
	inline const GLuint& GetProgramID_Debug() const { return programID_Debug; }

private:
	GLuint programID;
	GLuint matrixID;
	GLuint vertexArrayID;

	GLuint programID_Debug;
	GLuint matrixID_Debug;
	GLuint matrixID_Color_Debug;
};