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

	template<typename Func, typename Params>
	inline void CreateThreadFunction(const bool& _autoActivate, Func _method, Params _params)
	{
		bIsExecuted = _autoActivate;
		currentThread = CreateThread(0, 0, (Thread_Func)_method, _params, (_autoActivate ? 0 : CREATE_SUSPENDED), currentThreadID);
	}


public:
	Action<Thread*> OnFinished;

private:
	bool bIsExecuted;
	LPDWORD currentThreadID;
	HANDLE currentThread;
	
};