#include "Thread.h"


Thread::Thread(DWORD_PTR _threadAffinity)
{
	bIsExecuted = false;
	currentThread = nullptr;
	currentThreadID = nullptr;
	currentThreadAffinity = _threadAffinity;
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