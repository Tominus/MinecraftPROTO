#include "Thread_Manager.h"

#include "Thread_Obj.h"
#include <thread>

Thread_Manager::~Thread_Manager()
{
	for each (Thread_Obj* _threadObj in threadObjs)
	{
		delete _threadObj;
	}
}

void Thread_Manager::Initialize()
{
	maxSystemThread = std::thread::hardware_concurrency();
	Internal_SetMaxThread(Thread_Used_Qty);
	InitializeThreads();
}

void Thread_Manager::InitializeThreads()
{
	for (size_t i(0); i < currentThreadQuantityUsed; i++)
	{
		Thread_Obj* _threadObj = new Thread_Obj();

		_threadObj->OnFinishThread(
		[this](Thread_Obj* _thread)
		{
			SetThreadBehaviorFinished(_thread);
		});

		threadObjs.push_back(_threadObj);
		validThreadObjs.push_back(_threadObj);
	}
}

void Thread_Manager::SetMaxThread(const unsigned int& _quantity)
{
	Internal_SetMaxThread(_quantity);
	ResetThreadObjs();
}

void Thread_Manager::Internal_SetMaxThread(const unsigned int& _quantity)
{
	unsigned int _currentThreadQty(_quantity);

	if (_currentThreadQty > maxSystemThread)
	{
		_currentThreadQty = maxSystemThread;
	}

	currentThreadQuantityUsed = _currentThreadQty;
}

void Thread_Manager::ResetThreadObjs()
{
	printf("Thread_Manager::ResetThreadObjs -> TODO");
}

void Thread_Manager::WaitForEndOfThread(Thread_Obj* _thread) //TODO TEST
{
	const size_t& _max = invalidThreadObjs.size();

	for (size_t i(0); i < _max; ++i)
	{
		Thread_Obj* _invalidThread = invalidThreadObjs[i];
		if (_invalidThread == _thread)
		{
			//_thread->thread->join();
			//invalidThreadObjs.erase(invalidThreadObjs.begin() + i);
			//validThreadObjs.push_back(_thread);
			return;
		}
	}
	
	printf("Thread_Manager::WaitForEndOfThread -> Missing thread !!!");	
}

void Thread_Manager::SetThreadBehaviorFinished(Thread_Obj* _thread)
{
	WaitForEndOfThread(_thread);
}

Thread_Obj* Thread_Manager::GetThreadObj() //TODO TEST
{
	Thread_Obj* _tmpThread(nullptr);

	const size_t& _max = validThreadObjs.size();

	if (_max > 0)
	{
		 _tmpThread = validThreadObjs.at(_max - 1);
		validThreadObjs.pop_back();
		invalidThreadObjs.push_back(_tmpThread);
	}
	else
		printf("Thread_Manager::GetThreadObj -> Not enought valid thread");

	return _tmpThread;
}