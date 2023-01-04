#include "Thread_Manager.h"

#include "Thread_Obj.h"
#include <thread>

Thread_Manager::~Thread_Manager()
{
	InterruptThreadObjs();
	
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
	for (size_t i(0); i < currentThreadQuantityUsed; ++i)
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

void Thread_Manager::Internal_SetMaxThread(unsigned int _quantity)
{
	if (_quantity > maxSystemThread)
	{
		_quantity = maxSystemThread;
	}

	//currentThreadQuantityUsed = _quantity;
	currentThreadQuantityUsed = 16*16;
}

void Thread_Manager::ResetThreadObjs()
{
	/**
	* If you change the number of thread during game
	* - if you add thread it's ok we do nothing
	* - if you remove thread you have to wait them to join()
	*/
	printf("Thread_Manager::ResetThreadObjs -> TODO");
}

void Thread_Manager::InterruptThreadObjs()
{
	const size_t& _max = invalidThreadObjs.size();

	for (size_t i(0); i < _max; ++i)
	{
		invalidThreadObjs[i]->thread.detach();
	}
}

void Thread_Manager::DetachInvalidThread(Thread_Obj* _thread)
{
	mutex_InvalidThreadObjs.lock();
	const size_t& _max = invalidThreadObjs.size();

	for (size_t i(0); i < _max; ++i)
	{
		Thread_Obj* _invalidThread = invalidThreadObjs[i];
		if (_invalidThread == _thread)
		{
			if (_thread->thread.joinable())
				_thread->thread.join();
			//_thread->thread.detach();
			invalidThreadObjs.erase(invalidThreadObjs.begin() + i);
			validThreadObjs.push_back(_thread);
			mutex_InvalidThreadObjs.unlock();
			return;
		}
	}

	mutex_InvalidThreadObjs.unlock();
	printf("Thread_Manager::WaitForEndOfThread -> Missing thread !!!");	
}

void Thread_Manager::SetThreadBehaviorFinished(Thread_Obj* _thread)
{
	DetachInvalidThread(_thread);
}

Thread_Obj* Thread_Manager::GetValidThreadObj()
{
	Thread_Obj* _tmpThread(nullptr);

	mutex_InvalidThreadObjs.lock();
	const size_t& _max = validThreadObjs.size();

	if (_max > 0)
	{
		_tmpThread = validThreadObjs.at(_max - 1);
		validThreadObjs.pop_back();
		invalidThreadObjs.push_back(_tmpThread);
	}
	mutex_InvalidThreadObjs.unlock();

	return _tmpThread;
}