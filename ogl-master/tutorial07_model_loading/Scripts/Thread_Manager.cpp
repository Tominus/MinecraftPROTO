#include "Thread_Manager.h"

#include "Thread_Obj.h"

Thread_Manager::Thread_Manager()
{
	mutex = CreateMutex(0, false, 0);
}

Thread_Manager::~Thread_Manager()
{
	
}

void Thread_Manager::DeleteFinishedThread(Thread* _thread)
{
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = allCurrentThread.size();

	size_t i = 0;
	for (; i < _max; ++i)
	{
		if (_thread == allCurrentThread[i])
		{
			allCurrentThread.erase(allCurrentThread.begin() + i);
			break;
		}
	}
	if (i == _max)
		throw std::exception("Thread_Manager::DeleteFinishedThread -> Thread not found");

	_thread->OnFinished.RemoveDynamic(this, &Thread_Manager::DeleteFinishedThread);

	ReleaseMutex(mutex);

	delete _thread;
	_thread = nullptr;
}