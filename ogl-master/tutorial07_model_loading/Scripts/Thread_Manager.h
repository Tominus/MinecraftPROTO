#pragma once
#include "GlobalDefine.h"

#include <vector>
#include <Windows.h>

#include "Thread_Obj.h"

/**
* The Manager of thread.
* 
* Can be called by everyone to used thread 
*/
class Thread_Manager
{

	friend class MainGame;

private:
	Thread_Manager();
	~Thread_Manager();

public:
	inline static Thread_Manager& Instance()
	{
		static Thread_Manager instance;
		return instance;
	}

	bool GetAllThreadFinished() const { return allCurrentThread.size() == 0; }

	template<typename ... Params>
	inline Thread* CreateThread(const bool& _autoActivate, void(*_method), Params... _paramsWithoutThread)
	{
		Thread* _thread = new Thread();
		_thread->CreateThreadFunction(_method, _thread, _paramsWithoutThread...);

		_thread->OnFinished.AddDynamic(this, &Thread_Manager::DeleteFinishedThread);

		if (_autoActivate)
			_thread->Execute();

		WaitForSingleObject(mutex, INFINITE);

		allCurrentThread.push_back(_thread);

		ReleaseMutex(mutex);

		return _thread;
	}

private:
	void DeleteFinishedThread(Thread* _thread);

private:
	HANDLE mutex;

	std::vector<Thread*> allCurrentThread;
};