#include "Thread_Manager.h"

Thread_Manager::Thread_Manager()
{
	mutex = CreateMutex(0, false, 0);

	mainThread = GetCurrentThread();

	PGROUP_AFFINITY _groupAffinity = new GROUP_AFFINITY();
	GetThreadGroupAffinity(mainThread, _groupAffinity);

	mainThreadAffinity = _groupAffinity->Mask;
	delete _groupAffinity;

	mainThreadAffinity = 1;

	SetThreadAffinityMask(mainThread, mainThreadAffinity);

	if (!SetThreadPriority(mainThread, THREAD_PRIORITY_HIGHEST))
		throw std::exception("[Thread_Manager::Thread_Manager] -> Can't set Thread Manager priority");

	PPROCESSOR_NUMBER _processorNumber = new PROCESSOR_NUMBER();
	_processorNumber->Number = 0;
	
	if (!SetThreadIdealProcessorEx(mainThread, _processorNumber, nullptr))
		throw std::exception("[Thread_Manager::Thread_Manager] -> Can't set Thread Manager ideal proc");

	delete _processorNumber;
}

Thread_Manager::~Thread_Manager()
{
	ReleaseMutex(mutex);
}

Thread* Thread_Manager::CreateThread()
{
	Thread* _thread = new Thread(65534);
	_thread->OnFinished.AddDynamic(this, &Thread_Manager::DeleteFinishedThread);

	WaitForSingleObject(mutex, INFINITE);
	allCurrentThread.push_back(_thread);
	ReleaseMutex(mutex);

	return _thread;
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