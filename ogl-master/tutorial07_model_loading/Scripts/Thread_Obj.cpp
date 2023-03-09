#include "Thread_Obj.h"


Thread::Thread()
{
	currentThread = nullptr;
	currentThreadID = nullptr;
}

Thread::~Thread()
{
	
}

void Thread::Execute()
{
	ResumeThread(currentThread);
}
