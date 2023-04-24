#pragma once
#include "Action.h"
#include <Windows.h>

class Thread
{
	friend class Thread_Manager;

private:
	Thread() = delete;
	Thread(DWORD_PTR _threadAffinity);
	~Thread();

public:
	void Execute();

	template<typename Func, typename Params>
	inline void CreateThreadFunction(const bool& _autoActivate, Func _method, Params _params)
	{
		bIsExecuted = _autoActivate;
		currentThread = CreateThread(0, 0, (Thread_Func)_method, _params, CREATE_SUSPENDED, currentThreadID);
	
		SetThreadAffinityMask(currentThread, currentThreadAffinity);
		
		if (!SetThreadPriority(currentThread, THREAD_PRIORITY_NORMAL))
			throw std::exception("[Thread::CreateThreadFunction] -> Can't set Thread priority");

		PPROCESSOR_NUMBER _processorNumber = new PROCESSOR_NUMBER();
		GetThreadIdealProcessorEx(currentThread, _processorNumber);

		if (_processorNumber->Number == '\0')
		{
			_processorNumber->Number = (BYTE)(GetActiveProcessorCount(0) - 1);
			if (!SetThreadIdealProcessorEx(currentThread, _processorNumber, _processorNumber))
				throw std::exception("[Thread::CreateThreadFunction] -> Can't set Thread ideal proc");
		}
		delete _processorNumber;

		if (_autoActivate)
			ResumeThread(currentThread);
	}


public:
	Action<Thread*> OnFinished;

private:
	bool bIsExecuted;
	LPDWORD currentThreadID;
	HANDLE currentThread;
	DWORD_PTR currentThreadAffinity;
	
};