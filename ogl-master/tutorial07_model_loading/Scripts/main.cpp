#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <GL/glew.h>
#include <glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <time.h>

#include "GlobalDefine.h"
#include "MainGame.h"

#if ENABLE_DEBUG_MEMORY_LEAK
#define _CRTDBG_MAP_ALLOC
#endif ENABLE_DEBUG_MEMORY_LEAK

#if ENABLE_DEBUG_NEW_OVERRIDE
#define new new( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif ENABLE_DEBUG_NEW_OVERRIDE

int main( void )
{
	srand(time(NULL));

#if ENABLE_DEBUG_MEMORY_LEAK

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#if ENABLE_DEBUG_BREAK_ALLOC
	_CrtSetBreakAlloc(BREAK_ALLOC);
#endif ENABLE_DEBUG_BREAK_ALLOC

#endif ENABLE_DEBUG_MEMORY_LEAK
	
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		//getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//GLFWmonitor* _monitor = glfwGetPrimaryMonitor();
	window = glfwCreateWindow( 1024, 768, "Minecraft PROTO", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		//getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetWindowPos(window, 1920 / 2 - 1024/2, 1080 / 2 - 768/2);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		//getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	MainGame* _mainGame = new MainGame(window);

	_mainGame->GameLoop();

	delete _mainGame;

	glfwTerminate();

#if ENABLE_DEBUG_MEMORY_LEAK
	_CrtDumpMemoryLeaks();
#endif ENABLE_DEBUG_MEMORY_LEAK
	
	return 0;
}