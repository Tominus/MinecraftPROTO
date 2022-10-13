#include "Thread_Obj.h"
#include "World.h"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "Chunks_Manager.h"

Thread_Obj::Thread_Obj()
{
	//thread = new std::thread();
}

Thread_Obj::~Thread_Obj()
{
	//delete thread;
}

void Thread_Obj::TEST(Chunks_Manager* _chunkmanager)
{
	chunksManager = _chunkmanager;
	thread = std::thread(&Thread_Obj::MIAOU, this);
}

void Thread_Obj::MIAOU()
{
	chunksManager->AddChunk(glm::vec3(0, 0, 0));
	thread.detach();
}

void Thread_Obj::FinishThread()
{
	std::invoke(onFinishThread, this);
}