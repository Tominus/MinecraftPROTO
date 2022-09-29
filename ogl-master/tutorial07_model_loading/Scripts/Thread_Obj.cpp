#include "Thread_Obj.h"

Thread_Obj::Thread_Obj()
{
	thread = new std::thread();
}

Thread_Obj::~Thread_Obj()
{
	delete thread;
}

void Thread_Obj::FinishThread()
{
	std::invoke(callback, this);
}