#include "Thread_Manager.h"

#include "Thread_Obj.h"

Thread_Manager::~Thread_Manager()
{

}

void Thread_Manager::Initialize()
{
	maxSystemThread = std::thread::hardware_concurrency();
	SetMaxThread(Thread_Used_Qty);
	InitializeThreads();
}

void Thread_Manager::SetMaxThread(const unsigned int& _quantity)
{
	unsigned int _currentThreadQty(_quantity);

	if (_currentThreadQty > maxSystemThread)
	{
		_currentThreadQty = maxSystemThread;
	}

	currentThreadQuantityUsed = _currentThreadQty;
}

void Thread_Manager::InitializeThreads()
{
	/*threadObjs = std::vector<>;
	for each (Thread_Obj* _currentThread in threadObjs)
	{
		_currentThread = ;
	}*/
}