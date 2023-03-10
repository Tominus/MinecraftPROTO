#include "Thread_Obj.h"


Thread::Thread()
{
	bIsExecuted = false;
	currentThread = nullptr;
	currentThreadID = nullptr;
}

Thread::~Thread()
{
	
}

void Thread::Execute()
{
	if (bIsExecuted)
		throw std::exception("[Thread::Execute] -> Thread is already executed");

	ResumeThread(currentThread);
}
