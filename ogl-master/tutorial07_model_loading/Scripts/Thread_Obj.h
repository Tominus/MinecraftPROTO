#pragma once
#include "Action.h"
#include <Windows.h>

class Thread
{
	friend class Thread_Manager;

private:
	Thread();
	~Thread();

public:
	void Execute();
	
private:
	template<typename ... Params>
	inline void CreateThreadFunction(void(*_method), Params... _params) 
	{
		currentThread = CreateThread(0, 0, (Thread_Func)_method, _params..., CREATE_SUSPENDED, currentThreadID);
	}

public:
	Action<Thread*> OnFinished;

private:
	LPDWORD currentThreadID;
	HANDLE currentThread;
	
};