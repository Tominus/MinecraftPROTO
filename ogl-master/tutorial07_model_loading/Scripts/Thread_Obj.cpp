#include "Thread_Obj.h"
#include "World.h"

#include "Chunks_Manager.h"

Thread_Obj::Thread_Obj()
{
	//thread = new std::thread();
}

Thread_Obj::~Thread_Obj()
{
	//delete thread;
}

void Thread_Obj::TEST(Chunks_Manager* _chunkmanager, const glm::vec3& _loc)
{
	chunksManager = _chunkmanager;
	thread = std::thread(&Thread_Obj::MIAOU, this, _loc);
}

void Thread_Obj::MIAOU(const glm::vec3& _loc)
{
	chunksManager->AddChunk(_loc);
	FinishThread();
}

void Thread_Obj::FinishThread()
{
	std::invoke(onFinishThread, this);
}